#include <string>
#include <sstream>
#include <fstream>

#include "NodeManager.hh"

#include "Platform.hh"

#include "Maps.hh"

NodeManager::NodeManager() : irr::IReferenceCounted()
{
	localPlayer = NULL;
}

NodeManager::~NodeManager()
{
	size_t l = nodes.size();
	for(size_t i = 0; i < l; i++)
		nodes[i]->drop();

	nodes.clear();

	l = players.size();
        for(size_t i = 0; i < l; i++)
                players[i]->drop();

        players.clear();

	if(localPlayer)
		localPlayer->drop();
}

void NodeManager::updateAll(ms_t time)
{
	size_t l = nodes.size();

	std::vector<CNode*> firstNodes, lastNodes, anyNodes;

	for(size_t i = 0; i < l; i++) {
		switch(nodes[i]->getUpdateFlag())
		{
		case EUF_FIRST:
			firstNodes.push_back(nodes[i]);
			break;
		case EUF_LAST:
			lastNodes.push_back(nodes[i]);
			break;
		case EUF_ANY:
		default:
			anyNodes.push_back(nodes[i]);
			break;
		};
	}

	l = firstNodes.size();

	for(size_t i = 0; i < l; i++)
		firstNodes[i]->update(time);

	l = anyNodes.size();

	for(size_t i = 0; i < l; i++)
                anyNodes[i]->update(time);

	l = lastNodes.size();

	for(size_t i = 0; i < l; i++)
                lastNodes[i]->update(time);
}

void NodeManager::renderAll(irr::video::IVideoDriver *driver, Camera *camera)
{
	size_t l = nodes.size();

//	printf("%ld\n", l);

	for(size_t i = 0; i < l; i++)
		if(nodes[i]->shouldRender())
			nodes[i]->render(driver, camera);
}

void NodeManager::addNode(CNode *node)
{
	nodes.push_back(node);

	node->grab();
}

bool NodeManager::removeNode(CNode *node)
{
	size_t l = nodes.size();

	for(size_t i = 0; i < l; i++) {
		if(nodes[i] == node) {
			nodes[i]->drop();

			nodes.erase(nodes.begin() + i);

			//printf("Removed node: %ld\n", nodes.size());

			return true;
		}
	}

	return false;
}

void NodeManager::clearNodes()
{
	if(localPlayer) {
		localPlayer->drop();
		localPlayer = NULL;
	}

	size_t l = nodes.size();
	for(size_t i = 0; i < l; i++)
		nodes[i]->drop();

	l = players.size();
        for(size_t i = 0; i < l; i++)
                players[i]->drop();

	nodes.clear();
	players.clear();
}

bool NodeManager::addPlayer(playid_t pid, bool owner)
{
	Player *p = NULL;

	if((p = getPlayerByID(pid)))
		return false;
	else
		printf("Could not find player with id %d\n", pid);

	p = new Player(this, pid);

	addNode(p);

	players.push_back(p);
	p->grab();

	if(owner) {
		if(localPlayer)
			localPlayer->drop();

		localPlayer = p;
		p->grab();
	}

	p->drop();

	printf("Player added, id: %d  local: %d, total: %ld\n", pid, (int)owner, players.size());

	return true;
}

bool NodeManager::removePlayer(playid_t id)
{
	size_t l = players.size();

	//printf("Remove player\n");

        for(size_t i = 0; i < l; i++) {
                if(players[i]->getID() == id) {
			if(players[i] == localPlayer)
				localPlayer = NULL;

			//printf("Removed player\n");

			removeNode(players[i]);

//			players[i]->setVisible(false);

			players[i]->drop();
			players.erase(players.begin() + i);

			printf("Removed player, id: %d\n", id);

			return true;
		}
	}

	return false;
}

void NodeManager::addPlatform(Platform *plat)
{
        addNode(plat);

        platforms.push_back(plat);
        plat->grab();
}

bool NodeManager::removePlatform(Platform *plat)
{
        size_t l = platforms.size();

        for(size_t i = 0; i < l; i++) {
                if(platforms[i] == plat) {
                        removeNode(platforms[i]);

                        platforms[i]->drop();
                        platforms.erase(platforms.begin() + i);

                        return true;
                }
        }

        return false;
}

Player *NodeManager::getPlayerByID(playid_t id) const
{
	size_t l = players.size();

	for(size_t i = 0; i < l; i++)
		if(players[i]->getID() == id)
			return players[i];

	return NULL;
}

bool NodeManager::loadMap(const std::string& name)
{
	Map *map = Maps::getMap(name);

	if(!map)
		return false;

	std::vector<platform_t> plats = map->getPlatforms();
	size_t l = plats.size();

	for(size_t i = 0 ; i < l; i++) {
		Platform *p = new Platform(this, irr::core::position2d<irr::f32>(plats[i].X, plats[i].Y), irr::core::dimension2d<irr::f32>(plats[i].W, plats[i].H));
		addPlatform(p);
		p->drop();
	}

	return true;
}
