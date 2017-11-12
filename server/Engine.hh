#pragma once

#include <stdlib.h>
#include <vector>

#include "Player.hh"

#include "defines.hh"

class Engine
{
public:
	Engine();
	~Engine();

	void update();

	playid_t addPlayer(ip_t ip, double x, double y);
	bool removePlayer(playid_t id, ip_t ip);

	Player *getPlayerByID(playid_t id);

	inline std::vector<Player*> getPlayers() const { return players; }

private:
	playid_t nextID();

	playid_t lastID;

	std::vector<Player*> players;
};
