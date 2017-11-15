#pragma once

#include <irrlicht/irrlicht.h>

#include "CNode.hh"

class Platform : public CNode
{
public:
	Platform(class NodeManager *manager, const irr::core::position2d<irr::f32>& p, const irr::core::dimension2d<irr::f32> d);

	void update(ms_t time) { }

	void render(irr::video::IVideoDriver *driver, class Camera *camera);

	bool isInside(const irr::core::position2d<irr::f32>& p);
};
