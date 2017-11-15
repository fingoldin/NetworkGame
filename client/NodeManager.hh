#pragma once

#include <irrlicht/irrlicht.h>

#include <vector>
#include <string>

#include "CNode.hh"
#include "Player.hh"
#include "Camera.hh"
#include "Platform.hh"

class NodeManager : public irr::IReferenceCounted
{
public:
	NodeManager();
	~NodeManager();

	void updateAll(double time);
	void renderAll(irr::video::IVideoDriver *driver, Camera *camera);

	void addNode(CNode *node);
	bool removeNode(CNode *node);

	void addPlatform(Platform *plat);
	bool removePlatform(Platform *plat);

	void clearNodes();

	bool addPlayer(playid_t pid, bool owner);
	bool removePlayer(playid_t pid);

	Player *getPlayerByID(playid_t id) const;
	inline Player *getLocalPlayer() const { return localPlayer; }

	inline const std::vector<Player*>& getPlayers() const { return players; }
	inline const std::vector<Platform*>& getPlatforms() const { return platforms; }

	bool loadMap(const std::string& path);

private:

        std::vector<CNode*> nodes;
        std::vector<Player*> players;
	std::vector<Platform*> platforms;

	Player *localPlayer;
};
