#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <chrono>
#include <algorithm>

#include "Network.hh"

#include "Core.hh"

#include "EventReceiver.hh"

#include "Timer.hh"

#include "pack2.hh"

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

	printf("Created network thread\n");

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
	printf("Begun thread\n");

	if(!connected || !server_ip) {
		thread_running = false;
		pthread_exit(NULL);
	}

	thread_running = true;

	unsigned char buf[RECEIVE_LEN];
	ip_t ip;
	unsigned short port;

	size_t s_len = strlen(UPDATE_SIG);
	size_t cs_len = strlen(CONNECT_SIG);
	size_t ds_len = strlen(DISCONNECT_SIG);
	size_t pss_len = strlen(PLAYERS_SEND_SIG);
        size_t id_len = 2;
        size_t b_len = 4;

	bool firstLoop = true;
	ms_t last_time;

	ms_t lastServerMesTime = Timer::getMs();

	printf("Thread should run: %d\n", (int)thread_should_run);

	while(thread_should_run)
	{
		printf("Thread running\n");
		ssize_t r = receivePacket(buf, RECEIVE_LEN, &ip, &port, false);

//		if(r > -1)
//			printf("%ld bytes received\n", r);

		if(ip == server_ip && port == SERVER_PORT)
		{
			if((size_t)r >= (s_len + id_len + 2 * b_len))
			{
				if(strncmp((const char*)buf, UPDATE_SIG, s_len) == 0) {
					//playid_t pid = *(playid_t*)&buf[s_len];
					//n_block_t x = *(n_block_t*)&buf[s_len + id_len];
					//n_block_t y = *(n_block_t*)&buf[s_len + id_len + b_len];

					playid_t pid;
					double x, y;

					unpack(buf + s_len, "Hdd", &pid, &x, &y);

					playerUpdate(pid, x, y);

					lastServerMesTime = Timer::getMs();

					printf("Update: %f %f\n", x, y);
				}
			}
			else if((size_t)r == (cs_len + id_len + 2 * b_len) && strncmp((const char*)buf, CONNECT_SIG, cs_len) == 0) {
                                //playid_t pid = *(playid_t*)&buf[cs_len];
                                //n_block_t x = *(n_block_t*)&buf[cs_len + id_len];
                                //n_block_t y = *(n_block_t*)&buf[cs_len + id_len + b_len];

				playid_t pid;
				double x, y;

				unpack(buf + cs_len, "Hdd", &pid, &x, &y);

                               	if(addPlayer(pid, false))
	                               	playerUpdate(pid, (irr::f32)x, (irr::f32)y);

				lastServerMesTime = Timer::getMs();
			}
			else if((size_t)r == (ds_len + id_len) && strncmp((const char*)buf, DISCONNECT_SIG, ds_len) == 0) {
				playid_t pid;

				unpack(buf + ds_len, "H", &pid);

                                removePlayer(pid);

				lastServerMesTime = Timer::getMs();
                        }
			else if((size_t)r >= (pss_len) && (r - pss_len) % id_len == 0 && strncmp((const char*)buf, PLAYERS_SEND_SIG, pss_len) == 0) {
				size_t nids = std::min((r - pss_len) / id_len, (RECEIVE_LEN - pss_len) / id_len);
				playid_t *ids = (playid_t*)malloc(nids);

				NodeManager *m = core->getNodeManager();

				if(m)
				{
					for(size_t i = 0; i < nids; i++)
                                                unpack(buf + pss_len + id_len * i, "H", &ids[i]);

					//printf("%ld ids\n", nids);

					std::vector<Player*> players = m->getPlayers();
					size_t pl = players.size();

					std::vector<playid_t> toremove;

					for(size_t i = 0; i < pl; i++) {
						playid_t thisid = players[i]->getID();

						bool exists = false;

						for(size_t j = 0; j < nids; j++) {
							if(ids[j] == thisid) {
								exists = true;
								break;
							}
						}

						if(!exists)
							toremove.push_back(thisid);
					}

					size_t trl = toremove.size();

					for(size_t i = 0; i < trl; i++)
						removePlayer(toremove[i]);

					for(size_t i = 0; i < nids; i++)
                                                if(!m->getPlayerByID(ids[i]))
                                                        addPlayer(ids[i], false);

					lastServerMesTime = Timer::getMs();

					//printf("Received all player info: %.*s\n", (int)r, buf);
				}
			}
			else
				printf("Unrecognized packet received: %.*s\n", (int)r, buf);
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

				sendInputData();
			}

			ms_t dtServer = time - lastServerMesTime;

			if(dtServer > SERVER_TIMEOUT_TIME) {
				thread_running = false;

				disconnect();

				printf("Lost connection with server (timed out)\n");

				break;
			}
		}
	}

	thread_running = false;

	pthread_exit(NULL);
}

ssize_t Network::receivePacket(unsigned char *buf, size_t buf_len, ip_t *ip, unsigned short *port, bool nullTerminate=false)
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
		*port = ntohs(remote_addr.sin_port);

		return recvlen;
	}

	return -1;
}

bool Network::sendPacket(ip_t ip, unsigned char *buf, size_t buf_len)
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

std::string Network::connect(ip_t ip)
{
	if(!socket_init)
		return "";

	if(connected)
		disconnect();

	unsigned char buf[RECEIVE_LEN];
	ip_t s_ip;
	unsigned short port;

	ssize_t r;

	size_t sig_l = strlen(YOU_CONNECT_SIG);
	size_t b_l = 4;
	size_t i_len = 2;

	ms_t startMs = Timer::getMs();

	std::string map = "";

	while(true)
	{
		sendPacket(ip, (unsigned char*)CONNECT_MES, strlen(CONNECT_MES));

		if((r = receivePacket(buf, RECEIVE_LEN, &s_ip, &port, false)) > 0) {
			if((size_t)r > (sig_l + i_len + 2 * b_l) && s_ip == ip && port == SERVER_PORT && strncmp((char*)buf, YOU_CONNECT_SIG, sig_l) == 0) {
				connected = true;
				server_ip = ip;
				if(initThread()) {
					//playid_t pid = *(playid_t*)&buf[sig_l];
					//n_block_t x = *(n_block_t*)&buf[sig_l + i_len];
					//n_block_t y = *(n_block_t*)&buf[sig_l + i_len + b_l];

					playid_t pid;
					double x, y;

					unpack(buf + sig_l, "Hdd", &pid, &x, &y);

					map.reserve(r - sig_l - i_len - 2 * b_l);

					char *temp = (char*)malloc(r - sig_l - i_len - 2 * b_l + 1);
					strncpy(temp, (char*)(buf + sig_l + i_len + 2 * b_l), r - sig_l - i_len - 2 * b_l);

					map += temp;
					map += ".map";

					free(temp);

					printf("Map file: %s\n", map.c_str());
					//printf("You connect packet: %.*s\n", r, buf);

					addPlayer(pid, true);
					playerUpdate(pid, (irr::f32)x, (irr::f32)y);

					break;
				}
				else {
					disconnect();
					server_ip = 0;
					connected = false;
					break;
				}
			}
		}

		if((Timer::getMs() - startMs) > CONNECT_TIMEOUT_TIME)
			break;
		else
			usleep(USLEEP_TIME);
	}

	return map;
}

std::string Network::connect(const char *ip)
{
	struct sockaddr_in serv_addr;

	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) < 1)
		return "";

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
			size_t sig_l = strlen(YOU_DISCONNECT_SIG);
			size_t i_len = 2;

			unsigned char buf[dm_len + i_len];

			strcpy((char*)buf, DISCONNECT_MES);
			//*(playid_t*)&buf[dm_len] = p->getID();
			pack(buf + dm_len, "H", p->getID());

			ms_t startMs = Timer::getMs();

			unsigned char r_buf[RECEIVE_LEN];
			ip_t r_ip;
			unsigned short port;

			while(true)
			{
				//for(int i = 0; i < DISCONNECT_MES_NUM; i++)
				sendPacket(server_ip, buf, dm_len + i_len);

				size_t r = -1;

				if((r = receivePacket(r_buf, RECEIVE_LEN, &r_ip, &port, true)) > 0) {
					if((size_t)r == (sig_l + i_len) && r_ip == server_ip && port == SERVER_PORT && strncmp((char*)r_buf, YOU_DISCONNECT_SIG, sig_l) == 0) {
						playid_t id;

						unpack(r_buf + sig_l, "H", &id);

						if(id == p->getID())
							break;
					}
				}

				if((Timer::getMs() - startMs) > DISCONNECT_WAIT_TIME) {
					printf("Disconnect timeout\n");
                        		break;
				}
                		else
                        		usleep(USLEEP_TIME);
			}

			m->clearNodes();
		}
	}

	server_ip = 0;
	connected = false;
}

bool Network::addPlayer(playid_t pid, bool owner)
{
	NodeManager *m = core->getNodeManager();

        if(m)
		return m->addPlayer(pid, owner);

	return false;
}

bool Network::removePlayer(playid_t pid)
{
	NodeManager *m = core->getNodeManager();

	if(m)
		return m->removePlayer(pid);

	return false;
}

void Network::playerUpdate(playid_t pid, double x, double y)
{
	NodeManager *m = core->getNodeManager();

	Player *p = NULL;

	if(m)
		p = m->getPlayerByID(pid);

        if(p)
                p->setPosition(irr::core::position2d<irr::f32>((irr::f32)x, (irr::f32)y));
	else {
		addPlayer(pid, false);
		Player *np = m->getPlayerByID(pid);

		if(np)
			np->setPosition(irr::core::position2d<irr::f32>((irr::f32)x, (irr::f32)y));
	}

	//printf("Player update: %d %f %f\n", pid, x, y);
}

void Network::sendInputData()
{
	NodeManager *m = core->getNodeManager();
	Player *p = NULL;

	if(m)
		p = m->getLocalPlayer();

	EventReceiver *e = core->getEventReceiver();

	if(!m || !p || !server_ip || !e)
		return;

	size_t m_len = strlen(UPDATE_MES);
	size_t i_len = 2;
	size_t buf_len = m_len + i_len + 1;

	unsigned char buf[buf_len];

	strcpy((char*)buf, UPDATE_MES);
	//*(playid_t*)&buf[m_len] = p->getID();
	//buf[m_len + i_len] = (char)input;
	//buf[m_len + i_len + 1] = (char)state;

	uint8_t inputs = 0;

	for(int i = 0; i < EI_COUNT; i++) {
		inputs <<= 1;
		inputs |= (uint8_t)e->inputDown((E_INPUT)i);
	}

	pack(buf + m_len, "HC", p->getID(), inputs);

	printf("Sent output packet\n");
	sendPacket(server_ip, buf, buf_len);
}
