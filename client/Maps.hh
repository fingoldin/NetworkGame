#pragma once

#include <string>

#include <irrlicht/irrlicht.h>

#include "Map.hh"

class Maps
{
public:
	static void init();
	static void end();

	static Map *getMap(const std::string& path);

private:

	Maps() { }

	static std::vector<Map*> maps;
	static std::string mapsDir;
};
