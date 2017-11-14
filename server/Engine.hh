#pragma once

#include <stdlib.h>
#include <vector>
#include <string>

#include "Player.hh"
#include "Platform.hh"

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
	inline const std::vector<Platform*>& getPlatforms() const { return platforms; }

	bool setMap(const std::string& m);
	inline std::string getMap() const { return map; }

	inline void setThreadLock(bool s) { threadLock = s; }
	inline bool getThreadLock() const { return threadLock; }

private:
	bool loadMap(const std::string& path);

	void resetPlayers();
	void clearPlatforms();

	playid_t nextID();

	playid_t lastID;

	std::vector<Player*> players;
	std::vector<Platform*> platforms;

	std::string map;

	bool threadLock;
};
