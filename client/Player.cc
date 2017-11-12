#include "Player.hh"

#include "NodeManager.hh"

Player::Player(NodeManager *manager, playid_t id, ip_t ip) : CNode(manager), id(id), ip(ip), lastUpdateTime(-1.0)
{

}

void Player::update(double time)
{
	if(lastUpdateTime == -1.0)
		lastUpdateTime = time;

//	irr::u32 dt = time - lastUpdateTime;

	lastUpdateTime = time;
}

void Player::render(irr::video::IVideoDriver *driver)
{
	if(driver) {
		driver->draw2DRectangle(irr::video::SColor(255,255,255,255),
		irr::core::rect<irr::s32>(METERS_TO_PIXELS * pos.X - 20, METERS_TO_PIXELS * pos.Y - 20,
		METERS_TO_PIXELS * pos.X + 20, METERS_TO_PIXELS * pos.Y + 20));
	}
}
