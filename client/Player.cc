#include "Player.hh"

#include "NodeManager.hh"

#include "Camera.hh"

Player::Player(NodeManager *manager, playid_t id) : CNode(manager), id(id), lastUpdateTime(0)
{
	visible = true;
	box.set(PLAYER_WIDTH, PLAYER_HEIGHT);
}

void Player::update(ms_t time)
{
	if(lastUpdateTime == 0)
		lastUpdateTime = time;

//	irr::u32 dt = time - lastUpdateTime;

	lastUpdateTime = time;
}

void Player::render(irr::video::IVideoDriver *driver, Camera *camera)
{
//	printf("%f %f\n", pos.X, pos.Y);

	if(driver && camera) {
		irr::core::position2d<irr::f32> cpos = camera->getPos();
		irr::core::dimension2d<irr::f32> viewport = camera->getViewport();

		irr::s32 x1 = CORE_WINDOW_WIDTH / 2 + (irr::f32)CORE_WINDOW_WIDTH * (pos.X - box.Width - cpos.X) / viewport.Width;
		irr::s32 x2 = CORE_WINDOW_WIDTH / 2 + (irr::f32)CORE_WINDOW_WIDTH * (pos.X + box.Width - cpos.X) / viewport.Width;

		irr::s32 y1 = CORE_WINDOW_HEIGHT / 2 - (irr::f32)CORE_WINDOW_HEIGHT * (pos.Y + box.Height - cpos.Y) / viewport.Height;
		irr::s32 y2 = CORE_WINDOW_HEIGHT / 2 - (irr::f32)CORE_WINDOW_HEIGHT * (pos.Y - box.Height - cpos.Y) / viewport.Height;

		driver->draw2DRectangle(irr::video::SColor(255, 55, 55, 255), irr::core::rect<irr::s32>(x1, y1, x2, y2));
	}
}
