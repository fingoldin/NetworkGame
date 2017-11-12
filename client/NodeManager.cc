#include "NodeManager.hh"

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

	localPlayer->drop();
}

void NodeManager::updateAll(double time)
{
	size_t l = nodes.size();

	for(size_t i = 0; i < l; i++)
		nodes[i]->update(time);
}

void NodeManager::renderAll(irr::video::IVideoDriver *driver)
{
	size_t l = nodes.size();

	for(size_t i = 0; i < l; i++)
		nodes[i]->render(driver);
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

			return true;
		}
	}

	return false;
}

bool NodeManager::addPlayer(playid_t pid, ip_t ip, bool owner)
{
	Player *p = NULL;

	if((p = getPlayerByID(pid)))
		return false;

	p = new Player(this, pid, ip);

	addNode(p);

	players.push_back(p);
	p->grab();

	if(owner) {
		if(localPlayer)
			localPlayer->drop();

		localPlayer = p;
		p->grab();
	}

	printf("Player added, id: %d  local: %d\n", pid, (int)owner);

	return true;
}

bool NodeManager::removePlayer(playid_t id)
{
	size_t l = players.size();

        for(size_t i = 0; i < l; i++) {
                if(players[i]->getID() == id) {
			if(players[i] == localPlayer)
				localPlayer = NULL;

			removeNode(players[i]);

			players[i]->drop();
			players.erase(players.begin() + i);

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
