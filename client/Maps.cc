#include <fstream>
#include <sstream>

#include "Maps.hh"

#include "defines.hh"

std::vector<Map*> Maps::maps = {};
std::string Maps::mapsDir = "";

void Maps::init()
{
	mapsDir = MAP_DIR;

	Map *m1 = new Map("map1");

	m1->addPlatform(0.0, 0.0, 5.0, 1.0);
	m1->addPlatform(4.0, -2.0, 5.0, 1.0);
	m1->addPlatform(-4.0, -2.0, 5.0, 1.0);

	maps.push_back(m1);
}

Map *Maps::getMap(const std::string& name)
{
	size_t l = maps.size();

	for(size_t i = 0; i < l; i++)
		if(maps[i]->getName() == name)
			return maps[i];

	std::string p = mapsDir + name + ".map";
	std::ifstream infile(p.c_str());

	if(!infile.is_open()) {
		printf("Could not find map at path %s\n", p.c_str());
		return NULL;
	}

	Map *m = new Map(name);

	std::string line;
	while(std::getline(infile, line)) {
		std::istringstream iss(line);
		irr::f32 x, y, w, h;

		if(!(iss >> x >> y >> w >> h))
			break;

		m->addPlatform(x, y, w, h);
	}

	maps.push_back(m);

	return m;
}

void Maps::end()
{
	size_t l = maps.size();

	for(size_t i = 0; i < l; i++)
		maps[i]->drop();

	maps.clear();
}
