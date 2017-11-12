#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#include "Engine.hh"

#include "Network.hh"

bool Network::socket_init = false;
bool Network::thread_running = false;
bool Network::thread_should_run = false;
Engine *Network::engine = NULL;
int Network::n_fd = 0;
pthread_t Network::thread;

void Network::init(Engine *eng)
{
	socket_init = false;
	thread_running = false;
	thread_should_run = false;
	engine = eng;
	n_fd = 0;
	memset(&thread, 0, sizeof(pthread_t));

	if((n_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	printf("Error creating socket\n");
   	        return;
        }

        struct sockaddr_in n_addr;

        memset((char*)&n_addr, 0, sizeof(n_addr));

        n_addr.sin_family = AF_INET;
        n_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        n_addr.sin_port = htons(SERVER_PORT);

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

	if(pthread_create(&thread, NULL, Network::threadLoop, (void*)NULL) != 0) {
		thread_should_run = false;

		printf("Could not create network thread\n");
		return false;
	}

	printf("Thread created\n");

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
	thread_running = true;

	char buf[RECEIVE_LEN];
	ip_t ip;

	size_t um_len = strlen(UPDATE_MES);
	size_t cm_len = strlen(CONNECT_MES);
	size_t dm_len = strlen(DISCONNECT_MES);
	size_t cs_len = strlen(CONNECT_SIG);
	size_t ds_len = strlen(DISCONNECT_SIG);
	size_t ycs_len = strlen(YOU_CONNECT_SIG);
	size_t i_len = sizeof(playid_t);
	size_t b_l = sizeof(n_block_t);

	while(thread_should_run)
	{
		ssize_t r = receivePacket(buf, RECEIVE_LEN, &ip, true);

		if((size_t)r == (um_len + i_len + 2) && strncmp(buf, UPDATE_MES, um_len) == 0) {
			playid_t id = *(playid_t*)&buf[um_len];

			Player *p = engine->getPlayerByID(id);

			if(p && p->getIP() == ip) {
				E_INPUT input = (E_INPUT)buf[um_len + i_len];
				bool state = (bool)buf[um_len + i_len + 1];

				p->setInput(input, state);
			}

			printf("Update packet received: %s\n", buf);
		}
		else if((size_t)r == cm_len && strncmp(buf, CONNECT_MES, cm_len) == 0) {
			playid_t id = engine->addPlayer(ip, START_X, START_Y);

			size_t s_buf_len = cs_len + i_len + 2 * b_l;
			size_t ys_buf_len = ycs_len + i_len + 2 * b_l;
			char send_buf[s_buf_len];
			char you_send_buf[s_buf_len];

			strcpy(send_buf, CONNECT_SIG);
			*(playid_t*)&send_buf[cs_len] = id;
			*(n_block_t*)&send_buf[cs_len + i_len] = START_X;
			*(n_block_t*)&send_buf[cs_len + i_len + b_l] = START_Y;

			strcpy(you_send_buf, YOU_CONNECT_SIG);
			*(playid_t*)&you_send_buf[ycs_len] = id;
			*(n_block_t*)&you_send_buf[ycs_len + i_len] = START_X;
                        *(n_block_t*)&you_send_buf[ycs_len + i_len + b_l] = START_Y;

			std::vector<Player*> players = engine->getPlayers();
        		size_t l = players.size();

			for(size_t i = 0; i < l; i++) {
				if(players[i]->getID() == id && players[i]->getIP() == ip)
					for(int j = 0; j < CONNECT_SEND_TIMES; j++)
						sendPacket(players[i]->getIP(), you_send_buf, ys_buf_len);
				else {
					for(int j = 0; j < CONNECT_SEND_TIMES; j++)
						sendPacket(players[i]->getIP(), send_buf, s_buf_len);
				}
			}

			printf("Connect packet received: %s\n", buf);
		}
		else if((size_t)r == dm_len && strncmp(buf, DISCONNECT_MES, dm_len) == 0) {
			playid_t id = *(playid_t*)&buf[dm_len];

			std::vector<Player*> players = engine->getPlayers();
                        size_t l = players.size();

			bool exists = false;
                        for(size_t i = 0; i < l; i++) {
                                if(players[i]->getID() == id && players[i]->getIP() == ip) {
					exists = true;
					break;
				}
			}

			if(exists) {
				char buf[ds_len + i_len];

				strcpy(buf, DISCONNECT_SIG);
				*(playid_t*)&buf[ds_len] = id;

				for(size_t i = 0; i < l; i++)
					for(int j = 0; j < DISCONNECT_SEND_TIMES; j++)
						sendPacket(players[i]->getIP(), buf, ds_len + i_len);
			}

			printf("Disconnect packet received: %s\n", buf);
		}
		else if(r > -1)
			printf("Unrecognized packet received: %s\n", buf);
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
        dest_addr.sin_port = htons(CLIENT_PORT);
        dest_addr.sin_addr.s_addr = ip;

        return (sendto(n_fd, buf, buf_len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) > -1);
}

void Network::sendData()
{
	std::vector<Player*> players = engine->getPlayers();
	size_t l = players.size();

	size_t s_len = strlen(UPDATE_SIG);
	size_t id_len = sizeof(playid_t);
        size_t b_len = sizeof(n_block_t);
	size_t buf_len = s_len + id_len + 2 * b_len;

	char buf[buf_len];

	strcpy(buf, UPDATE_SIG);

	for(size_t i = 0; i < l; i++) {
		if(players[i]->hasChanged()) {
			*(playid_t*)&buf[s_len] = players[i]->getID();
			*(n_block_t*)&buf[s_len + id_len] = (n_block_t)players[i]->getX();
			*(n_block_t*)&buf[s_len + id_len + b_len] = (n_block_t)players[i]->getY();

			sendPacket(players[i]->getIP(), buf, buf_len);

			players[i]->setHasChanged(false);
		}
	}
}
