#include "Player.hh"

Player::Player(playid_t id, ip_t ip) : id(id), ip(ip), lastUpdateTime(-1.0), x_pos(0.0), y_pos(0.0), changed(true), lastSignalTime(0), lastSendTime(0)
{
}

void Player::update(ms_t time)
{
	if(lastUpdateTime == -1.0)
		lastUpdateTime = time;

	double dt = 0.001 * (double)(time - lastUpdateTime);

	if(inputs[EI_LEFT])
		setX(x_pos - PLAYER_MOVE_SPEED * dt);
	if(inputs[EI_RIGHT])
		setX(x_pos + PLAYER_MOVE_SPEED * dt);

	if(inputs[EI_UP])
                setY(y_pos + PLAYER_MOVE_SPEED * dt);
        if(inputs[EI_DOWN])
                setY(y_pos - PLAYER_MOVE_SPEED * dt);

	lastUpdateTime = time;
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
