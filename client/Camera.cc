#include "Camera.hh"

#include "NodeManager.hh"

Camera::Camera(NodeManager *mgr, const irr::core::position2df& pos, irr::f32 max) : CNode(mgr), pos(pos)
{
        if(CORE_WINDOW_WIDTH == CORE_WINDOW_HEIGHT)
                viewport.set(max, max);
        else if(CORE_WINDOW_WIDTH > CORE_WINDOW_HEIGHT)
                viewport.set(max, max * (irr::f32)CORE_WINDOW_HEIGHT / (irr::f32)CORE_WINDOW_WIDTH);
        else
                viewport.set(max * (irr::f32)CORE_WINDOW_WIDTH / (irr::f32)CORE_WINDOW_HEIGHT, max);

	updateFlag = EUF_LAST;
}

void Camera::update(ms_t)
{
	if(!manager)
		return;

	Player *p = manager->getLocalPlayer();

	if(!p)
		return;

	pos = p->getPosition();

}

const irr::core::position2d<irr::f32>& Camera::getPos()
{
	if(!manager)
                return pos;

        Player *p = manager->getLocalPlayer();

        if(!p)
                return pos;

        return p->getPosition();
}
