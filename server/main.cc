#include <unistd.h>

#include "Network.hh"

#include "Timer.hh"

int main(int argc, char **argv)
{
	Timer::init();

	Engine engine;

	Network::init(&engine);
	Network::initThread();

	while(true)
	{
		engine.update();

		Network::sendData();
	}

	Network::closeThread();

	return 0;
}
