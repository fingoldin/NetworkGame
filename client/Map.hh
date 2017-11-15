#pragma once

#include <string>
#include <vector>

#include <irrlicht/irrlicht.h>

#include "defines.hh"

struct platform_t {
        platform_t(irr::f32 x, irr::f32 y, irr::f32 w, irr::f32 h) : X(x), Y(y), W(w), H(h) { }

        irr::f32 X, Y, W, H;
};

class Map : public irr::IReferenceCounted
{
public:
	Map(const std::string& n);

	inline const std::string& getName() const { return name; }
	inline const std::vector<platform_t>& getPlatforms() const { return platforms; }

	void addPlatform(irr::f32, irr::f32 y, irr::f32 w, irr::f32);

private:
	std::string name;

	std::vector<platform_t> platforms;
};
