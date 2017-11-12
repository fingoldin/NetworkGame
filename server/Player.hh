#pragma once

#include "defines.hh"

class Player
{
public:
	Player(playid_t id, ip_t ip);

	void update(double time);

	const playid_t getID() { return id; }
	const ip_t getIP() { return ip; }

	inline void setInput(E_INPUT input, bool state) { inputs[input] = state; }

	inline double getX() const { return x_pos; }
	inline double getY() const { return y_pos; }

	void setX(double x);
        void setY(double y);

	inline bool hasChanged() const { return changed; }
	inline void setHasChanged(bool s) { changed = s; }

//	const E_ANIM getAnim() { return anim; }
//	const double getAnimPos() { return animPos; }

//	void setAnim(E_ANIM a);
//	void setAnimPos(double p);

private:

	bool inputs[EI_COUNT];

	playid_t id;
	ip_t ip;

//	E_ANIM anim;
//	double animPos;

	double lastUpdateTime;

	double x_pos;
	double y_pos;

	bool changed;
};
