#pragma once

#ifdef _WINDOWS_
	#include <windows.h>
	#include <winsock2.h>
#endif

#include <pthread.h>
#include <string>

#include "defines.hh"

class Network
{
public:
	static void init(class Core *core);
	static void end();

	static std::string connect(const char *ip);
	static std::string connect(ip_t ip);
	static void disconnect();

	static void sendInputData();

private:
	Network() { }

	static bool initThread();
	static void closeThread();
	static void *threadLoop(void *args);

	static ssize_t receivePacket(unsigned char *buf, size_t buf_len, ip_t *ip, unsigned short *port, bool nullTerminate);
	static bool sendPacket(ip_t ip, unsigned char *buf, size_t buf_len);

	static bool addPlayer(playid_t pid, bool owner);
	static bool removePlayer(playid_t pid);
	static void playerUpdate(playid_t pid, uint8_t inputs, double x, double y, double xv, double yv);

	static bool socket_init;
	static bool thread_running;
	static bool thread_should_run;
	static bool connected;

	static ip_t server_ip;

#ifdef _WINDOWS_
	SOCKET n_fd;
	HANDLE thread;
#else
	static int n_fd;
	static pthread_t thread;
#endif

	static class Core *core;
};
