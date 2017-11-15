#include <math.h>

#include "Player.hh"

#include "NodeManager.hh"

#include "Camera.hh"

Player::Player(NodeManager *manager, playid_t id) : CNode(manager), id(id), lastUpdateTime(0), packetSince(false), rightFacing(false)
{
	visible = true;

	box.set(PLAYER_WIDTH, PLAYER_HEIGHT);
	vel.set(0.0, 0.0);
	hitbox.set(PLAYER_HITBOX_W, PLAYER_HITBOX_H);

	//spawnTime = Timer::getMs();

	for(int i = 0; i < EI_COUNT; i++) {
                inputs[i] = false;
		lastInputs[i] = false;
	}
}

void Player::update(ms_t time)
{
	if(lastUpdateTime == 0)
		lastUpdateTime = time;

	irr::f32 dt = 0.001 * (irr::f32)(time - lastUpdateTime);

//	printf("%d\n", (bool)inputs[EI_LEFT]);

//	printf("dt: %f\n", dt);

        if(!packetSince) {
                irr::f32 speed = PLAYER_SPEED;
                irr::f32 acc = PLAYER_ACC;
                irr::f32 decc = PLAYER_DECC;

                Platform *plat;

                //printf("%d\n", (bool)inputs[EI_JUMP]);

                if(!(plat = onGround())) {
                        speed = PLAYER_AIR_SPEED;
                        acc = PLAYER_AIR_ACC;
                        decc = PLAYER_AIR_DECC;

                        vel.Y -= dt * GRAVITY;
                }
                else {
                        //printf("%d %d\n", (int)inputs[EI_JUMP], (int)lastJump);
                        if(inputs[EI_JUMP] && !lastJump) {
                                speed = PLAYER_AIR_SPEED;
                                acc = PLAYER_AIR_ACC;
                                decc = PLAYER_AIR_DECC;

                                plat = NULL;

                                vel.Y = PLAYER_JUMP_IMPULSE;
                        }
                        else {
                                vel.Y = 0.0;
                                pos.Y = plat->getPosition().Y + plat->getBox().Height / 2.0 + PLAYER_HEIGHT / 2.0;
                        }
                }

                bool moved = false;

                if(inputs[EI_LEFT] && vel.X > -speed) {
                        vel.X -= acc * dt;

                        moved = true;

                        if(vel.X < -speed)
                                vel.X = -speed;
                }

                if(inputs[EI_RIGHT] && vel.X < speed) {
                        vel.X += acc * dt;

                        moved = true;

                        if(vel.X > speed)
                                vel.X = speed;
                }

                if(!moved && vel.X != 0.0) {
                        irr::f32 sign = 1.0;
                        if(vel.X < 0.0)
                                sign = -1.0;

                        vel.X -= sign * dt * decc;

                        if(vel.X * sign < 0.0)
                                vel.X = 0.0;
                }

		if(manager && inputs[EI_PUNCH] && !lastInputs[EI_PUNCH]) {
                        std::vector<Player*> players = manager->getPlayers();
                        size_t l = players.size();

                        for(size_t i = 0; i < l; i++)
                                punchPlayer(players[i]);
                }

                pos.X += vel.X * dt;
                pos.Y += vel.Y * dt;
        }

	if(vel.X > 0.0)
                rightFacing = true;
        else if(vel.X < 0.0)
                rightFacing = false;

	packetSince = false;

	lastUpdateTime = time;

	for(int i = 0; i < EI_COUNT; i++)
                lastInputs[i] = inputs[i];
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
        if(manager && vel.Y <= 0.0) {
                std::vector<Platform*> platforms = manager->getPlatforms();
                size_t pl = platforms.size();

                for(size_t i = 0; i < pl; i++)
                        if(platforms[i]->isInside(pos + irr::core::position2d<irr::f32>(-PLAYER_WIDTH / 2.0, -PLAYER_GROUND)) ||
                          platforms[i]->isInside(pos + irr::core::position2d<irr::f32>(PLAYER_WIDTH / 2.0, -PLAYER_GROUND)))
                                return platforms[i];
        }

        return NULL;
}

void Player::punchPlayer(Player *player)
{
        if(!player || player == this || !manager)
                return;

	irr::core::position2d<irr::f32> col;

        col.Y = pos.Y;

        if(rightFacing)
                col.X = col.X + PLAYER_PUNCH;
        else
                col.X = col.X - PLAYER_PUNCH;

       	irr::f32 m = 1.0;

        if(!rightFacing)
                m = -1.0;

        if(player->inHitbox(col)) {
                irr::f32 f = atan((player->getPosition().Y - pos.Y) / (player->getPosition().X - pos.X)) + PLAYER_PUNCH_ANGLE;

		printf("Punched player locally\n");

                player->applyImpulse(irr::core::position2d<irr::f32>(m * PLAYER_PUNCH_IMPULSE * cos(f), PLAYER_PUNCH_IMPULSE * sin(f)));
        }
}

bool Player::inHitbox(const irr::core::position2d<irr::f32>& p)
{
        irr::f32 x1 = pos.X + hitbox.Width / 2.0;
        irr::f32 y1 = pos.Y + hitbox.Height / 2.0;
        irr::f32 x2 = pos.X - hitbox.Width / 2.0;
        irr::f32 y2 = pos.Y - hitbox.Height / 2.0;

        return (p.X < x1 && p.X > x2 && p.Y < y1 && p.Y > y2);
}

void Player::applyImpulse(const irr::core::position2d<irr::f32>& i)
{
	vel += i;
}
