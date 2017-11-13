#pragma once

#include <pthread.h>

#include "Engine.hh"

#include "defines.hh"

class Network
{
public:
	static void init(Engine *eng);
	static void end();

	static void sendData();

	static bool initThread();
        static void closeThread();

private:
	Network() { }

	static void *threadLoop(void *args);

	static ssize_t receivePacket(unsigned char *buf, size_t buf_len, ip_t *ip, unsigned short *port, bool nullTerminate);
	static bool sendPacket(ip_t ip, unsigned char *buf, size_t buf_len);

	static bool socket_init;
	static bool thread_running;
	static bool thread_should_run;

	static pthread_t thread;
	static int n_fd;

	static class Engine *engine;
};
