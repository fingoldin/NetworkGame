#include "Platform.hh"

#include "NodeManager.hh"

#include "Camera.hh"

Platform::Platform(NodeManager *manager, const irr::core::position2d<irr::f32>& p, const irr::core::dimension2d<irr::f32> d) : CNode(manager)
{
	visible = true;
	pos = p;
	box = d;
}

void Platform::render(irr::video::IVideoDriver *driver, Camera *camera)
{
	if(driver && camera) {
		irr::core::position2d<irr::f32> cpos = camera->getPos();
                irr::core::dimension2d<irr::f32> viewport = camera->getViewport();

                irr::s32 x1 = CORE_WINDOW_WIDTH / 2 + (irr::f32)CORE_WINDOW_WIDTH * (pos.X - box.Width / 2.0 - cpos.X) / viewport.Width;
                irr::s32 x2 = CORE_WINDOW_WIDTH / 2 + (irr::f32)CORE_WINDOW_WIDTH * (pos.X + box.Width / 2.0 - cpos.X) / viewport.Width;

                irr::s32 y1 = CORE_WINDOW_HEIGHT / 2 - (irr::f32)CORE_WINDOW_HEIGHT * (pos.Y + box.Height / 2.0 - cpos.Y) / viewport.Height;
                irr::s32 y2 = CORE_WINDOW_HEIGHT / 2 - (irr::f32)CORE_WINDOW_HEIGHT * (pos.Y - box.Height / 2.0 - cpos.Y) / viewport.Height;

                driver->draw2DRectangle(irr::video::SColor(255, 255, 255, 255), irr::core::rect<irr::s32>(x1, y1, x2, y2));
        }
}
