#include "Map.hh"

Map::Map(const std::string& n) : name(n)
{

}

void Map::addPlatform(irr::f32 x, irr::f32 y, irr::f32 w, irr::f32 h)
{
	platforms.push_back(platform_t(x, y, w, h));
}
