#include "Player.hh"

#include "Engine.hh"

#include "Timer.hh"

Player::Player(Engine * eng, playid_t id, ip_t ip) : eng(eng), id(id), ip(ip), x_pos(0.0), y_pos(0.0), x_vel(0.0), y_vel(0.0), changed(true), lastUpdateTime(0), lastSignalTime(0), lastSendTime(0), lastJump(false)
{
	//spawnTime = Timer::getMs();

	for(int i = 0; i < EI_COUNT; i++)
		inputs[i] = false;
}

void Player::update(ms_t time)
{
	if(lastUpdateTime == 0)
		lastUpdateTime = time;

	double dt = 0.001 * (double)(time - lastUpdateTime);

//	printf("dt: %f\n", dt);

	/*if(inputs[EI_UP])
                setY(y_pos + PLAYER_MOVE_SPEED * dt);
        if(inputs[EI_DOWN])
                setY(y_pos - PLAYER_MOVE_SPEED * dt);*/

	//if((time - spawnTime) > IMMUNE_TIME) {
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
				setY(plat->getY() + plat->getH() / 2.0 + PLAYER_HEIGHT / 2.0);
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

		setX(x_pos + x_vel * dt);
		setY(y_pos + y_vel * dt);
	//}

	lastUpdateTime = time;

	lastJump = inputs[EI_JUMP];
}

Platform *Player::onGround()
{
	if(eng && y_vel <= 0.0) {
		std::vector<Platform*> platforms = eng->getPlatforms();
		size_t pl = platforms.size();

		for(size_t i = 0; i < pl; i++)
			if(platforms[i]->isInside(x_pos - PLAYER_WIDTH / 2.0, y_pos - PLAYER_GROUND) ||
			  platforms[i]->isInside(x_pos + PLAYER_WIDTH / 2.0, y_pos - PLAYER_GROUND))
				return platforms[i];
	}

	return NULL;
}

void Player::setX(double x)
{
	if(x_pos != x)
		changed = true;

	x_pos = x;
}

void Player::setY(double y)
{
	if(y_pos != y)
		changed = true;

	y_pos = y;
}
