#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include "Network.hh"

#include "Timer.hh"

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(int argc, char **argv)
{
	signal(SIGINT, intHandler);

	printf("Starting up\n");

	Timer::init();

	Engine engine("map1");

	Network::init(&engine);
	Network::initThread();

	while(keepRunning)
	{
		engine.update();

		Network::sendData();

		usleep(10000);
	}

	printf("\nClosing down\n");

	Network::closeThread();

	return 0;
}
