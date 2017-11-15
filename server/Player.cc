#include <math.h>

#include "Player.hh"

#include "Engine.hh"

#include "Timer.hh"

Player::Player(Engine * eng, playid_t id, ip_t ip) : eng(eng), id(id), ip(ip), x_pos(0.0), y_pos(0.0), x_vel(0.0), y_vel(0.0), hitboxW(PLAYER_HITBOX_W), hitboxH(PLAYER_HITBOX_H), changed(true), lastUpdateTime(0), lastSignalTime(0), lastSendTime(0), rightFacing(true)
{
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

	double dt = 0.001 * (double)(time - lastUpdateTime);

//	printf("%d, %d\n", id, (int)rightFacing);

//	printf("%d\n", (int)inputs[EI_LEFT]);

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
			if(inputs[EI_JUMP] && !lastInputs[EI_JUMP]) {
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

		if(eng && inputs[EI_PUNCH] && !lastInputs[EI_PUNCH]) {
			std::vector<Player*> players = eng->getPlayers();
			size_t l = players.size();

			for(size_t i = 0; i < l; i++)
				punchPlayer(players[i]);
		}

//		printf("%f %f\n", x_vel, y_vel);

		setX(x_pos + x_vel * dt);
		setY(y_pos + y_vel * dt);
	//}

	if(x_vel > 0.0)
		rightFacing = true;
	else if(x_vel < 0.0)
		rightFacing = false;

	lastUpdateTime = time;

	for(int i = 0; i < EI_COUNT; i++)
		lastInputs[i] = inputs[i];
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

void Player::punchPlayer(Player *player)
{
	if(!player || player == this || !eng)
		return;

	double x_col, y_col;

	y_col = y_pos;

	if(rightFacing)
		x_col = x_pos + PLAYER_PUNCH;
	else
		x_col = x_pos - PLAYER_PUNCH;

	double m = 1.0;

	if(!rightFacing)
		m = -1.0;

	if(player->inHitbox(x_col, y_col)) {
		double f = atan((player->getY() - y_pos) / (player->getX() - x_pos)) + PLAYER_PUNCH_ANGLE;

		player->applyImpulse(m * PLAYER_PUNCH_IMPULSE * cos(f), PLAYER_PUNCH_IMPULSE * sin(f));
	}
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

bool Player::inHitbox(double x, double y)
{
	double x1 = x_pos + hitboxW / 2.0;
	double y1 = y_pos + hitboxH / 2.0;
	double x2 = x_pos - hitboxW / 2.0;
	double y2 = y_pos - hitboxH / 2.0;

	return (x < x1 && x > x2 && y < y1 && y > y2);
}

void Player::applyImpulse(double x, double y)
{
	x_vel += x;
	y_vel += y;
}
