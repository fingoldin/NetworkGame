#pragma once

#include "defines.hh"

#include "Platform.hh"

class Player
{
public:
	Player(class Engine *eng, playid_t id, ip_t ip);

	void update(ms_t time);

	const playid_t getID() { return id; }
	const ip_t getIP() { return ip; }

	inline bool getInput(E_INPUT input) const { return inputs[input]; }
	inline void setInput(E_INPUT input, bool state) { inputs[input] = state; }

	inline double getX() const { return x_pos; }
	inline double getY() const { return y_pos; }

	inline double getXVel() const { return x_vel; }
	inline double getYVel() const { return y_vel; }

	void setX(double x);
        void setY(double y);

	inline bool hasChanged() const { return changed; }
	inline void setHasChanged(bool s) { changed = s; }

	void setLastSignalTime(ms_t t) { if(t > lastSignalTime) lastSignalTime = t; }
	inline ms_t getLastSignalTime() const { return lastSignalTime; }

	void setLastSendTime(ms_t t) { if(t > lastSendTime) lastSendTime = t; }
        inline ms_t getLastSendTime() const { return lastSendTime; }

//	const E_ANIM getAnim() { return anim; }
//	const double getAnimPos() { return animPos; }

//	void setAnim(E_ANIM a);
//	void setAnimPos(double p);

protected:

	Platform *onGround();

	bool inputs[EI_COUNT];

	class Engine *eng;

	playid_t id;
	ip_t ip;

//	E_ANIM anim;
//	double animPos;

	double x_pos, y_pos, x_vel, y_vel;

	bool changed;

	ms_t lastUpdateTime;
	ms_t lastSignalTime;
	ms_t lastSendTime;
//	ms_t spawnTime;

	bool lastJump;
};
