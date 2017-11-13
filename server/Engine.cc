#include "Engine.hh"

#include "Timer.hh"

Engine::Engine(const std::string& m) : map(m)
{
	lastID = 0;
}

Engine::~Engine()
{
	size_t l = players.size();

	for(size_t i = 0; i < l; i++)
		delete players[i];
}

playid_t Engine::addPlayer(ip_t ip, double x, double y)
{
	playid_t id = nextID();

	Player *p =new Player(id, ip);

	p->setX(x);
	p->setY(y);

	players.push_back(p);

	return id;
}

bool Engine::removePlayer(playid_t id, ip_t ip)
{
	size_t l = players.size();

        for(size_t i = 0; i < l; i++) {
		if(players[i]->getIP() == ip && players[i]->getID() == id) {
			delete players[i];
			players.erase(players.begin() + i);
			return true;
		}
	}

	return false;
}

Player *Engine::getPlayerByID(playid_t id)
{
	size_t l = players.size();

        for(size_t i = 0; i < l; i++)
		if(players[i]->getID() == id)
			return players[i];

	return NULL;
}

Player *Engine::getPlayerByIP(ip_t ip)
{
        size_t l = players.size();

        for(size_t i = 0; i < l; i++)
                if(players[i]->getIP() == ip)
                        return players[i];

        return NULL;
}

playid_t Engine::nextID()
{
	return ++lastID;
}

void Engine::update()
{
	size_t l = players.size();

	ms_t time = Timer::getMs();

	for(size_t i = 0; i < l; i++)
		players[i]->update(time);
}
