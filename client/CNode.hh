#pragma once

#include <irrlicht/irrlicht.h>

#include "defines.hh"

enum E_UPDATE_FLAG {
	EUF_FIRST = 0,
	EUF_LAST,
	EUF_ANY,
	EUF_COUNT
};

class CNode : public irr::IReferenceCounted
{
public:
	CNode(class NodeManager *manager);
	~CNode();

	virtual void update(ms_t time) = 0;
	virtual void render(irr::video::IVideoDriver *driver, class Camera *camera) = 0;

	virtual bool shouldRender() { return visible; }

	virtual inline const irr::core::position2d<irr::f32>& getPosition() const { return pos; }
	virtual inline void setPosition(const irr::core::position2d<irr::f32>& p) { pos = p; }

	virtual inline bool isVisible() { return visible; }
	virtual inline void setVisible(bool v) { visible = v; }

	virtual inline const irr::core::dimension2d<irr::f32>& getBox() const { return box; }
	virtual inline void setBox(const irr::core::dimension2d<irr::f32>& b) { box = b; }

	virtual inline const E_UPDATE_FLAG getUpdateFlag() const { return updateFlag; }

protected:
	bool visible;

	irr::core::position2d<irr::f32> pos;
	irr::core::dimension2d<irr::f32> box;

	class NodeManager *manager;

	E_UPDATE_FLAG updateFlag;
};
