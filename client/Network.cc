#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <chrono>

#include "Network.hh"

#include "Core.hh"

#include "EventReceiver.hh"

#include "Timer.hh"

bool Network::socket_init = false;
bool Network::thread_running = false;
bool Network::thread_should_run = false;
int Network::n_fd = 0;
pthread_t Network::thread;
Core *Network::core = NULL;
bool Network::connected = false;
ip_t Network::server_ip = 0;

void Network::init(Core *co)
{
	core = co;
	socket_init = false;
        thread_running = false;
        thread_should_run = false;
        n_fd = 0;
	connected = false;
	server_ip = 0;
	memset(&thread, 0, sizeof(pthread_t));

	if((n_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	printf("Error creating socket\n");
   	        return;
        }

        struct sockaddr_in n_addr;

        memset((char*)&n_addr, 0, sizeof(n_addr));

        n_addr.sin_family = AF_INET;
        n_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        n_addr.sin_port = htons(CLIENT_PORT);

        if(bind(n_fd, (struct sockaddr*)&n_addr, sizeof(n_addr)) < 0) {
                printf("Socket binding failed\n");
                return;
        }

        struct timeval tv;
        tv.tv_sec = RECEIVE_TIMEOUT_S;
        tv.tv_usec = RECEIVE_TIMEOUT_US;
        if (setsockopt(n_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                printf("Could not set timeout\n");
                return;
        }

	socket_init = true;
}

void Network::end()
{
	if(socket_init)
		close(n_fd);
}

bool Network::initThread()
{
	if(thread_running)
		return true;

	thread_should_run = true;

	if(pthread_create(&thread, NULL, threadLoop, (void*)NULL) != 0) {
		thread_should_run = false;

		printf("Could not create network thread\n");
		return false;
	}

	return true;
}

void Network::closeThread()
{
	if(thread_running) {
                thread_should_run = false;

        	while(thread_running) { usleep(USLEEP_TIME); }
	}
}

void *Network::threadLoop(void *args)
{
	if(!connected || !server_ip) {
		thread_running = false;
		pthread_exit(NULL);
	}

	thread_running = true;

	char buf[RECEIVE_LEN];
	ip_t ip;

	size_t s_len = strlen(UPDATE_SIG);
	size_t cs_len = strlen(CONNECT_SIG);
	size_t ds_len = strlen(DISCONNECT_SIG);
        size_t id_len = sizeof(playid_t);
        size_t b_len = sizeof(n_block_t);

	bool firstLoop = true;
	ms_t last_time;

	while(thread_should_run)
	{
		ssize_t r = receivePacket(buf, RECEIVE_LEN, &ip, false);

//		if(r > -1)
//			printf("%ld bytes received\n", r);

		if(ip == server_ip)
		{
			if((size_t)r >= (s_len + id_len + 2 * b_len))
			{
				if(strncmp((const char*)buf, UPDATE_SIG, s_len) == 0) {
					playid_t pid = *(playid_t*)&buf[s_len];
					n_block_t x = *(n_block_t*)&buf[s_len + id_len];
					n_block_t y = *(n_block_t*)&buf[s_len + id_len + b_len];

					playerUpdate(pid, x, y);
				}
			}
			else if((size_t)r == (cs_len + id_len + 2 * b_len) && strncmp(buf, CONNECT_SIG, cs_len) == 0) {
                                playid_t pid = *(playid_t*)&buf[cs_len];
                                n_block_t x = *(n_block_t*)&buf[cs_len + id_len];
                                n_block_t y = *(n_block_t*)&buf[cs_len + id_len + b_len];

                               	if(addPlayer(pid, ip, false))
	                               	playerUpdate(pid, (irr::f32)x, (irr::f32)y);
			}
			else if((size_t)r == (ds_len + id_len) && strncmp(buf, DISCONNECT_SIG, ds_len) == 0) {
                                playid_t pid = *(playid_t*)&buf[ds_len];

                                removePlayer(pid);
                        }
		}

		if(firstLoop) {
			last_time = Timer::getMs();
			firstLoop = false;
		}
		else {
			ms_t time = Timer::getMs();

			ms_t dt = time - last_time;

			if(dt > SEND_ALL_TIME) {
				last_time = time;

				EventReceiver *r = core->getEventReceiver();

				if(r) {
					for(int input = 0; input < EI_COUNT; input++)
						sendInputData((E_INPUT)input, r->inputDown((E_INPUT)input));
				}
			}
		}
	}

	thread_running = false;

	pthread_exit(NULL);
}

ssize_t Network::receivePacket(char *buf, size_t buf_len, ip_t *ip, bool nullTerminate=false)
{
	if(!socket_init)
		return -1;

	struct sockaddr_in remote_addr;
        socklen_t addr_len = sizeof(remote_addr);

	size_t len = nullTerminate ? (buf_len - 1) : buf_len;

        ssize_t recvlen = recvfrom(n_fd, buf, len, 0, (struct sockaddr*)&remote_addr, &addr_len);

	if(recvlen == 0) {
		printf("Peer has performed orderly shutdown\n");
		return 0;
	}
	else if(recvlen != -1) {
		if(nullTerminate)
			buf[len] = '\0';

		*ip = remote_addr.sin_addr.s_addr;

		return recvlen;
	}

	return -1;
}

bool Network::sendPacket(ip_t ip, char *buf, size_t buf_len)
{
	if(!socket_init)
		return false;

	struct sockaddr_in dest_addr;
        memset((char*)&dest_addr, 0, sizeof(dest_addr));
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(SERVER_PORT);
        dest_addr.sin_addr.s_addr = ip;

        return (sendto(n_fd, buf, buf_len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) > -1);
}

bool Network::connect(ip_t ip)
{
	if(!socket_init)
		return false;

	if(connected)
		disconnect();

	sendPacket(ip, (char*)CONNECT_MES, strlen(CONNECT_MES));

	char buf[RECEIVE_LEN];
	ip_t s_ip;

	ssize_t r;

	int i = 0;

	size_t sig_l = strlen(YOU_CONNECT_SIG);
	size_t b_l = sizeof(n_block_t);
	size_t i_len = sizeof(playid_t);

	while(true)
	{
		if((r = receivePacket(buf, RECEIVE_LEN, &s_ip, false)) > 0) {
			if((size_t)r == (sig_l + i_len + 2 * b_l) && s_ip == ip && strncmp(buf, YOU_CONNECT_SIG, sig_l) == 0) {
				if(initThread()) {
					connected = true;

					playid_t pid = *(playid_t*)&buf[sig_l];
					n_block_t x = *(n_block_t*)&buf[sig_l + i_len];
					n_block_t y = *(n_block_t*)&buf[sig_l + i_len + b_l];

					server_ip = ip;

					addPlayer(pid, ip, true);
					playerUpdate(pid, (irr::f32)x, (irr::f32)y);
					break;
				}
				else {
					disconnect();
					connected = false;
					break;
				}
			}
		}

		if(i++ > CONNECT_TIMEOUT_TIME / USLEEP_TIME)
			break;

		usleep(USLEEP_TIME);
	}

	return connected;
}

bool Network::connect(const char *ip)
{
	struct sockaddr_in serv_addr;

	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) < 1)
		return false;

	return connect((ip_t)serv_addr.sin_addr.s_addr);
}

void Network::disconnect()
{
	if(thread_running)
		closeThread();

	NodeManager *m = core->getNodeManager();

	if(m && socket_init && server_ip) {
		Player *p = m->getLocalPlayer();

		if(p) {
			size_t dm_len = strlen(DISCONNECT_MES);
			size_t i_len = sizeof(playid_t);

			char buf[dm_len + i_len];

			strcpy(buf, DISCONNECT_MES);
			*(playid_t*)&buf[dm_len] = p->getID();

			removePlayer(p->getID());

			for(int i = 0; i < DISCONNECT_MES_NUM; i++)
				sendPacket(server_ip, buf, dm_len + i_len);

		}
	}

	connected = false;
}

bool Network::addPlayer(playid_t pid, ip_t ip, bool owner)
{
	NodeManager *m = core->getNodeManager();

        if(m)
		return m->addPlayer(pid, ip, owner);

	return false;
}

bool Network::removePlayer(playid_t pid)
{
	NodeManager *m = core->getNodeManager();

	if(m)
		return m->removePlayer(pid);

	return false;
}

void Network::playerUpdate(playid_t pid, n_block_t x, n_block_t y)
{
	NodeManager *m = core->getNodeManager();

	Player *p = NULL;

	if(m)
		p = m->getPlayerByID(pid);

        if(p)
                p->setPosition(irr::core::position2d<irr::f32>((irr::f32)x, (irr::f32)y));

	printf("Player update: %d %f %f\n", pid, x, y);
}

void Network::sendInputData(E_INPUT input, bool state)
{
	NodeManager *m = core->getNodeManager();
	Player *p = NULL;

	if(m)
		p = m->getLocalPlayer();


	if(!m || !p || !server_ip)
		return;

	size_t m_len = strlen(UPDATE_MES);
	size_t i_len = sizeof(playid_t);
	size_t buf_len = m_len + i_len + 2;

	char buf[buf_len];

	strcpy(buf, UPDATE_MES);
	*(playid_t*)&buf[m_len] = p->getID();
	buf[m_len + i_len] = (char)input;
	buf[m_len + i_len + 1] = (char)state;

	sendPacket(server_ip, buf, buf_len);
}
