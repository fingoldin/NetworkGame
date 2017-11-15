#pragma once

#include <irrlicht/irrlicht.h>

#include "CNode.hh"
#include "Camera.hh"
#include "Platform.hh"

#include "defines.hh"

class Player : public CNode
{
public:
	Player(class NodeManager *manager, playid_t id);

	void update(ms_t time);

	void render(irr::video::IVideoDriver *driver, Camera *camera);

	inline void setInput(E_INPUT input, bool s) { inputs[input] = s; }

	inline void setVel(const irr::core::position2d<irr::f32>& v) { vel = v; }

	inline void setPacketSince(bool s) { packetSince = s; }

	inline playid_t getID() const { return id; }

	void punchPlayer(Player *player);

	bool inHitbox(const irr::core::position2d<irr::f32>& p);

	void applyImpulse(const irr::core::position2d<irr::f32>& i);

//	inline ip_t getIP() const { return ip; }
//	inline void setIP(ip_t i) { ip = i; }

//	const E_ANIM getAnim() { return anim; }
//	const double getAnimPos() { return animPos; }

//	void setAnim(E_ANIM a);
//	void setAnimPos(double p);

protected:

	Platform *onGround();

	bool inputs[EI_COUNT];
	bool lastInputs[EI_COUNT];

	playid_t id;
//	ip_t ip;

//	E_ANIM anim;
//	double animPos;

	ms_t lastUpdateTime;

	irr::core::position2d<irr::f32> vel;

	irr::core::dimension2d<irr::f32> hitbox;

	bool lastJump;
	bool packetSince;

	bool rightFacing;
};
