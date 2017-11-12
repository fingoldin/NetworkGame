#pragma once

#include <irrlicht/irrlicht.h>

#include <vector>

#include "CNode.hh"
#include "Player.hh"

class NodeManager : public irr::IReferenceCounted
{
public:
	NodeManager();
	~NodeManager();

	void updateAll(double time);
	void renderAll(irr::video::IVideoDriver *driver);

	void addNode(CNode *node);
	bool removeNode(CNode *node);

	bool addPlayer(playid_t pid, ip_t ip, bool owner);
	bool removePlayer(playid_t pid);

	Player *getPlayerByID(playid_t id) const;
	inline Player *getLocalPlayer() const { return localPlayer; }

private:

        std::vector<CNode*> nodes;
        std::vector<Player*> players;

	Player *localPlayer;
};
