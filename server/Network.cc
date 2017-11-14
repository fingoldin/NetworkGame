#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#include "Engine.hh"

#include "Network.hh"

#include "pack2.hh"

#include "Timer.hh"

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

		printf("Closed thread\n");
	}
}

void *Network::threadLoop(void *args)
{
	thread_running = true;

	unsigned char buf[RECEIVE_LEN];
	ip_t ip;
	unsigned short port;

	size_t um_len = strlen(UPDATE_MES);
	size_t cm_len = strlen(CONNECT_MES);
	size_t dm_len = strlen(DISCONNECT_MES);
	size_t cs_len = strlen(CONNECT_SIG);
	size_t ds_len = strlen(DISCONNECT_SIG);
	size_t ycs_len = strlen(YOU_CONNECT_SIG);
	size_t yds_len = strlen(YOU_DISCONNECT_SIG);
	size_t pss_len = strlen(PLAYERS_SEND_SIG);
	size_t i_len = 2;
	size_t b_l = 4;

	ms_t last_players_send_time = Timer::getMs();

	while(thread_should_run)
	{
		ssize_t r = receivePacket(buf, RECEIVE_LEN, &ip, &port, true);

		if(port != CLIENT_PORT)
			continue;

		ms_t time = Timer::getMs();

		if((size_t)r == (um_len + i_len + 1) && strncmp((const char*)buf, UPDATE_MES, um_len) == 0) {
//			playid_t id = *(playid_t*)&buf[um_len];

			printf("Update received\n");

			playid_t id;
			uint8_t inputs;

			unpack(buf + um_len, "HC", &id, &inputs);

			Player *p = engine->getPlayerByID(id);

			if(p && p->getIP() == ip) {
				for(int i = (EI_COUNT - 1); i >= 0; i--) {
					p->setInput((E_INPUT)i, (bool)(inputs & 1));
					inputs >>= 1;
				}

				p->setLastSignalTime(time);
			}

			printf("Update packet received\n");
		}
		else if(((size_t)r == cm_len && strncmp((char*)buf, CONNECT_MES, cm_len) == 0) || ((size_t)r == (dm_len + i_len) && strncmp((char*)buf, DISCONNECT_MES, dm_len) == 0)) {
			size_t s_buf_len = cs_len + i_len + 2 * b_l;
			size_t ys_buf_len = ycs_len + i_len + 2 * b_l;
			unsigned char send_buf[SEND_LEN];
			unsigned char you_send_buf[SEND_LEN];

			//strcpy((char*)you_send_buf, YOU_CONNECT_SIG);
                        //(playid_t*)&you_send_buf[ycs_len] = id;
                        //(n_block_t*)&you_send_buf[ycs_len + i_len] = START_X;
                        //(n_block_t*)&you_send_buf[ycs_len + i_len + b_l] = START_Y;

			bool disconnect = false;
			bool sendAll = false;

			playid_t id;

			if((size_t)r == (dm_len + i_len) && strncmp((char*)buf, DISCONNECT_MES, dm_len) == 0) {
				disconnect = true;

				unpack(buf + dm_len, "H", &id);
			}

			bool skip = false;

			std::string map = engine->getMap();

			Player *p = engine->getPlayerByIP(ip);
			if(!p)
			{
				// Connect message, player doesn't exist on server
				if(!disconnect)
				{
					//engine->setThreadLock(true);
					id = engine->addPlayer(ip, START_X, START_Y);

					strcpy((char*)send_buf, CONNECT_SIG);
					strcpy((char*)you_send_buf, YOU_CONNECT_SIG);
					//(playid_t*)&send_buf[cs_len] = id;
					//(n_block_t*)&send_buf[cs_len + i_len] = START_X;
					//(n_block_t*)&send_buf[cs_len + i_len + b_l] = START_Y;

					pack(send_buf + cs_len, "Hdd", id, START_X, START_Y);

					pack(you_send_buf + ycs_len, "Hdd", id, START_X, START_Y);
					strcpy((char*)(you_send_buf + ycs_len + i_len + 2 * b_l), map.c_str());
					ys_buf_len += map.length();

					//printf("%ld\n", strlen(map.c_str()));

					sendAll = true;
				}
				// Disconnect message, player doesn't exist on server
				else
				{
					strcpy((char*)you_send_buf, YOU_DISCONNECT_SIG);
					pack(you_send_buf + yds_len, "H", id);

					ys_buf_len = yds_len + i_len;
				}
			}
			// Connect message, player exists on server
			else if(!disconnect) {
				strcpy((char*)you_send_buf, YOU_CONNECT_SIG);
				pack(you_send_buf + ycs_len, "Hdd", id, p->getX(), p->getY());

				strcpy((char*)(you_send_buf + ycs_len + i_len + 2 * b_l), map.c_str());
				ys_buf_len += map.length();

				printf("Special message len: %ld (%ld)\n", ys_buf_len, map.length());

				p->setLastSignalTime(time);
			}
			// Disconnect message, player exists on server
			else {
				if(id != p->getID())
					skip = true;
				else
				{
					strcpy((char*)send_buf, DISCONNECT_SIG);
					pack(send_buf + ds_len, "H", id);

					strcpy((char*)you_send_buf, YOU_DISCONNECT_SIG);
					pack(you_send_buf + yds_len, "H", id);

					s_buf_len = ds_len + i_len;
					ys_buf_len = yds_len + i_len;

					sendAll = true;

					p->setLastSignalTime(time);

					sendPacket(p->getIP(), you_send_buf, ys_buf_len);

					//engine->setThreadLock(true);
					engine->removePlayer(p->getID(), p->getIP());
				}
			}

			if(!skip)
			{
				std::vector<Player*> players = engine->getPlayers();
        			size_t l = players.size();

				if(sendAll)
				{
					for(size_t i = 0; i < l; i++) {
						if(players[i]->getID() == id) {
							sendPacket(players[i]->getIP(), you_send_buf, ys_buf_len);
							players[i]->setLastSignalTime(time);
						}
						else {
							//for(int j = 0; j < CONNECT_SEND_TIMES; j++)
							sendPacket(players[i]->getIP(), send_buf, s_buf_len);
						}
					}
				}
				else
					sendPacket(ip, you_send_buf, ys_buf_len);

				printf("Disconnect/Connect packet received: %s\n", (char*)buf);

				//sendData();
			}
			else
				printf("Corrupt disconnect packet received: %s\n", (char*)buf);
		}
		/*else if((size_t)r == (dm_len + i_len) && strncmp((const char*)buf, DISCONNECT_MES, dm_len) == 0) {
			//playid_t id = *(playid_t*)&buf[dm_len];

			playid_t id;

			unpack(buf + dm_len, "H", &id);

			if(engine->removePlayer(id, ip)) {
				unsigned char s_buf[ds_len + i_len];

				strcpy((char*)s_buf, DISCONNECT_SIG);
				//(playid_t*)&s_buf[ds_len] = id;

				pack(s_buf + ds_len, "H", id);

				std::vector<Player*> players = engine->getPlayers();
				size_t l = players.size();

				for(size_t i = 0; i < l; i++)
					//for(int j = 0; j < DISCONNECT_SEND_TIMES; j++)
					sendPacket(players[i]->getIP(), s_buf, ds_len + i_len);
			}

			printf("Disconnect packet received: %s\n", (char*)buf);
		}*/
		else if(r > -1)
			printf("Unrecognized packet received: %s\n", (char*)buf);

		if((time - last_players_send_time) > PLAYERS_SEND_TIME) {
			last_players_send_time = time;

			std::vector<Player*> players = engine->getPlayers();
                        size_t l = players.size();

			if(l)
			{
				size_t ps_buf_len = pss_len + 2 * l;
				unsigned char *ps_buf = (unsigned char*)malloc(ps_buf_len);

				strcpy((char*)ps_buf, PLAYERS_SEND_SIG);

				for(size_t i = 0; i < l; i++)
					pack(ps_buf + pss_len + 2 * i, "H", players[i]->getID());

                	        for(size_t i = 0; i < l; i++)
                        	        //for(int j = 0; j < DISCONNECT_SEND_TIMES; j++)
                        	        sendPacket(players[i]->getIP(), ps_buf, ps_buf_len);

				//printf("All player ids packet sent: %.*s\n", (int)ps_buf_len, (char*)ps_buf);

				free(ps_buf);
			}
		}

		std::vector<Player*> players = engine->getPlayers();
		size_t pl = players.size();

		for(size_t i = 0; i < pl; i++) {
			if((time - players[i]->getLastSignalTime()) > PLAYER_TIMEOUT_TIME) {
				playid_t plid = players[i]->getID();
				ip_t plip = players[i]->getIP();

				//engine->setThreadLock(true);
				if(engine->removePlayer(plid, plip)) {
					unsigned char s_buf[yds_len + i_len];
					unsigned char y_s_buf[yds_len + i_len];

                        	        strcpy((char*)s_buf, DISCONNECT_SIG);
                               		pack(s_buf + ds_len, "H", plid);

					strcpy((char*)y_s_buf, YOU_DISCONNECT_SIG);
					pack(y_s_buf + yds_len, "H", plid);

                                	std::vector<Player*> players2 = engine->getPlayers();
                                	size_t pl2 = players2.size();

                                	for(size_t j = 0; j < pl2; j++) {
                                        	//for(int j = 0; j < DISCONNECT_SEND_TIMES; j++)
                                        	sendPacket(players2[j]->getIP(), s_buf, ds_len + i_len);
					}

					sendPacket(plip, y_s_buf, yds_len + i_len);

					printf("Disconnected player %d: timeout\n", plid);
				}
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
        dest_addr.sin_port = htons(CLIENT_PORT);
        dest_addr.sin_addr.s_addr = ip;

        return (sendto(n_fd, buf, buf_len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) > -1);
}

void Network::sendData()
{
	if(!socket_init)
		return;

	std::vector<Player*> players = engine->getPlayers();
	size_t l = players.size();

	size_t s_len = strlen(UPDATE_SIG);
	size_t id_len = 2;
        size_t b_len = 4;
	size_t buf_len = s_len + id_len + 2 * b_len;

	unsigned char buf[buf_len];

	strcpy((char*)buf, UPDATE_SIG);

	ms_t time = Timer::getMs();

//	printf("Send begin\n");

	for(size_t i = 0; i < l; i++) {
//		if(engine->getThreadLock()) {
//			engine->setThreadLock(false);
//			break;
//		}

		if(players[i]->hasChanged() || (time - players[i]->getLastSendTime()) > PLAYER_SEND_TIME) {
			//(playid_t*)&buf[s_len] = players[i]->getID();
			//(n_block_t*)&buf[s_len + id_len] = (n_block_t)players[i]->getX();
			//(n_block_t*)&buf[s_len + id_len + b_len] = (n_block_t)players[i]->getY();

			players[i]->setLastSendTime(time);

			pack(buf + s_len, "Hdd", players[i]->getID(), players[i]->getX(), players[i]->getY());

			//printf("Sent update packet %d\n", players[i]->getID());

			for(size_t j = 0; j < l; j++)
				sendPacket(players[j]->getIP(), buf, buf_len);

			players[i]->setHasChanged(false);
		}
	}

//	printf("Send end\n");
}
