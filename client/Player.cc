#include "Player.hh"

#include "NodeManager.hh"

#include "Camera.hh"

Player::Player(NodeManager *manager, playid_t id) : CNode(manager), id(id), lastUpdateTime(0), x_vel(0.0), y_vel(0.0), lastJump(false), packetSince(false)
{
	visible = true;
	box.set(PLAYER_WIDTH, PLAYER_HEIGHT);

	//spawnTime = Timer::getMs();

	for(int i = 0; i < EI_COUNT; i++)
                inputs[i] = false;
}

void Player::update(ms_t time)
{
	if(lastUpdateTime == 0)
		lastUpdateTime = time;

	double dt = 0.001 * (double)(time - lastUpdateTime);

	printf("dt: %f\n", dt);

        if(!packetSince) {
                double speed = PLAYER_SPEED;
                double acc = PLAYER_ACC;
                double decc = PLAYER_DECC;

                Platform *plat;

                //printf("%d\n", (bool)inputs[EI_JUMP]);

                if(!(plat = onGround())) {
                        speed = PLAYER_AIR_SPEED;
                        acc = PLAYER_AIR_ACC;
                        decc = PLAYER_AIR_DECC;

                        y_vel -= dt * GRAVITY;
                }
                else {
                        //printf("%d %d\n", (int)inputs[EI_JUMP], (int)lastJump);
                        if(inputs[EI_JUMP] && !lastJump) {
                                speed = PLAYER_AIR_SPEED;
                                acc = PLAYER_AIR_ACC;
                                decc = PLAYER_AIR_DECC;

                                plat = NULL;

                                y_vel = PLAYER_JUMP_IMPULSE;
                        }
                        else {
                                y_vel = 0.0;
                                pos.Y = plat->getPosition().Y + plat->getBox().Height / 2.0 + PLAYER_HEIGHT / 2.0;
                        }
                }

                bool moved = false;

                if(inputs[EI_LEFT] && x_vel > -speed) {
                        x_vel -= acc * dt;

                        moved = true;

                        if(x_vel < -speed)
                                x_vel = -speed;
                }

                if(inputs[EI_RIGHT] && x_vel < speed) {
                        x_vel += acc * dt;

                        moved = true;

                        if(x_vel > speed)
                                x_vel = speed;
                }

                if(!moved && x_vel != 0.0) {
                        double sign = 1.0;
                        if(x_vel < 0.0)
                                sign = -1.0;

                        x_vel -= sign * dt * decc;

                        if(x_vel * sign < 0.0)
                                x_vel = 0.0;
                }

                pos.X += x_vel * dt;
                pos.Y += y_vel * dt;
        }

	packetSince = false;

	lastUpdateTime = time;
}

void Player::render(irr::video::IVideoDriver *driver, Camera *camera)
{
//	printf("%f %f\n", pos.X, pos.Y);

	if(driver && camera) {
		irr::core::position2d<irr::f32> cpos = camera->getPos();
		irr::core::dimension2d<irr::f32> viewport = camera->getViewport();

		irr::s32 x1 = CORE_WINDOW_WIDTH / 2 + (irr::f32)CORE_WINDOW_WIDTH * (pos.X - box.Width / 2.0 - cpos.X) / viewport.Width;
		irr::s32 x2 = CORE_WINDOW_WIDTH / 2 + (irr::f32)CORE_WINDOW_WIDTH * (pos.X + box.Width / 2.0 - cpos.X) / viewport.Width;

		irr::s32 y1 = CORE_WINDOW_HEIGHT / 2 - (irr::f32)CORE_WINDOW_HEIGHT * (pos.Y + box.Height / 2.0 - cpos.Y) / viewport.Height;
		irr::s32 y2 = CORE_WINDOW_HEIGHT / 2 - (irr::f32)CORE_WINDOW_HEIGHT * (pos.Y - box.Height / 2.0 - cpos.Y) / viewport.Height;

		driver->draw2DRectangle(irr::video::SColor(255, 55, 55, 255), irr::core::rect<irr::s32>(x1, y1, x2, y2));
	}
}

Platform *Player::onGround()
{
        if(manager && y_vel <= 0.0) {
                std::vector<Platform*> platforms = manager->getPlatforms();
                size_t pl = platforms.size();

                for(size_t i = 0; i < pl; i++)
                        if(platforms[i]->isInside(pos + irr::core::position2d<irr::f32>(-PLAYER_WIDTH / 2.0, -PLAYER_GROUND)) ||
                          platforms[i]->isInside(pos + irr::core::position2d<irr::f32>(PLAYER_WIDTH / 2.0, -PLAYER_GROUND)))
                                return platforms[i];
        }

        return NULL;
}
