#pragma once

#include <irrlicht/irrlicht.h>

#include "CNode.hh"
#include "Camera.hh"

#include "defines.hh"

class Player : public CNode
{
public:
	Player(class NodeManager *manager, playid_t id);

	void update(ms_t time);

	void render(irr::video::IVideoDriver *driver, Camera *camera);

	inline playid_t getID() const { return id; }
//	inline ip_t getIP() const { return ip; }
//	inline void setIP(ip_t i) { ip = i; }

//	const E_ANIM getAnim() { return anim; }
//	const double getAnimPos() { return animPos; }

//	void setAnim(E_ANIM a);
//	void setAnimPos(double p);

protected:

	playid_t id;
//	ip_t ip;

//	E_ANIM anim;
//	double animPos;

	ms_t lastUpdateTime;
};
