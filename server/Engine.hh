#pragma once

#include <stdlib.h>
#include <vector>
#include <string>

#include "Player.hh"

#include "defines.hh"

class Engine
{
public:
	Engine(const std::string& m);
	~Engine();

	void update();

	playid_t addPlayer(ip_t ip, double x, double y);
	bool removePlayer(playid_t id, ip_t ip);
	bool removePlayer(Player *p);

	Player *getPlayerByID(playid_t id);
	Player *getPlayerByIP(ip_t ip);

	inline const std::vector<Player*>& getPlayers() const { return players; }

	inline void setMap(const std::string& m) { map = m; }
	inline std::string getMap() const { return map; }

private:
	playid_t nextID();

	playid_t lastID;

	std::vector<Player*> players;

	std::string map;
};
