#pragma once

#include <pthread.h>

#include "defines.hh"

class Network
{
public:
	static void init(class Core *core);
	static void end();

	static bool connect(const char *ip);
	static bool connect(ip_t ip);
	static void disconnect();

	static void sendInputData(E_INPUT input, bool state);

private:
	Network() { }

	static bool initThread();
	static void closeThread();
	static void *threadLoop(void *args);

	static ssize_t receivePacket(char *buf, size_t buf_len, ip_t *ip, bool nullTerminate);
	static bool sendPacket(ip_t ip, char *buf, size_t buf_len);

	static bool addPlayer(playid_t pid, ip_t ip, bool owner);
	static bool removePlayer(playid_t pid);
	static void playerUpdate(playid_t pid, n_block_t x, n_block_t y);

	static bool socket_init;
	static bool thread_running;
	static bool thread_should_run;
	static bool connected;

	static ip_t server_ip;

	static pthread_t thread;
	static int n_fd;

	static class Core *core;
};
