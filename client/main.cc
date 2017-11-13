#include "Core.hh"

int main(int argc, char * argv[])
{
	Core core;

	core.begin("Irrlicht");

/*	if(!Network::connect("127.0.0.1")) {
		printf("Could not connect\n");
		return 1;
	}
*/
	while(core.run())
	{
		if(core.active()) {
			core.update();

			core.render();
		}
		else {
			core.yield();
		}
	}

	Network::disconnect();

	core.end();

	return 0;
};

