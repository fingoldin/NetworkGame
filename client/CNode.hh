#pragma once

#include <irrlicht/irrlicht.h>

class CNode : public irr::IReferenceCounted
{
public:
	CNode(class NodeManager *manager);
	~CNode();

	virtual void update(double time) = 0;
	virtual void render(irr::video::IVideoDriver *driver) = 0;

	virtual bool shouldRender() { return visible; }

	virtual inline const irr::core::position2d<irr::f32>& getPosition() const { return pos; }
	virtual inline void setPosition(const irr::core::position2d<irr::f32>& p) { pos = p; }

	virtual inline bool isVisible() { return visible; }
	virtual inline void setVisible(bool v) { visible = v; }

protected:
	bool visible;

	irr::core::position2d<irr::f32> pos;

	class NodeManager *manager;
};
