#pragma once

#include <irrlicht/irrlicht.h>

#include "CNode.hh"

#include "defines.hh"

class Camera : public CNode
{
public:
	Camera(class NodeManager *mgr, const irr::core::position2df& pos, irr::f32 max);

	const irr::core::position2d<irr::f32>& getPos();
	inline void setPos(const irr::core::position2d<irr::f32>& p) { pos = p; }

	inline const irr::core::dimension2d<irr::f32>& getViewport() const { return viewport; }

	void update(ms_t time);
	void render(irr::video::IVideoDriver *driver, Camera *camera) { }

private:
	irr::core::position2d<irr::f32> pos;

	irr::core::dimension2d<irr::f32> viewport;
};
