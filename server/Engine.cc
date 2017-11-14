#include <fstream>
#include <string>
#include <sstream>

#include "Engine.hh"
#include "Network.hh"
#include "Timer.hh"

Engine::Engine(const std::string& m) :threadLock(false)
{
	lastID = 0;

	setMap(m);
}

Engine::~Engine()
{
	size_t l = players.size();

	for(size_t i = 0; i < l; i++)
		delete players[i];

	l = platforms.size();

	for(size_t i = 0; i < l; i++)
		delete platforms[i];
}

playid_t Engine::addPlayer(ip_t ip, double x, double y)
{
	playid_t id = nextID();

	Player *p = new Player(this, id, ip);

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

//	printf("Update begin\n");

	for(size_t i = 0; i < l; i++) {
//		if(threadLock) {
//			threadLock = false;
//			break;
//		}

//		printf("Update %d\n", players[i]->getID());

		players[i]->update(time);
	}

//	printf("Update end\n");

	//printf("Num players: %ld\n", players.size());
}

bool Engine::setMap(const std::string& m)
{
	if(m != "" && loadMap(m)) {
		map = m;
		return true;
	}

	printf("Could not load map\n");
	return false;
}

void Engine::clearPlatforms()
{
	size_t l = platforms.size();

	for(size_t i = 0; i < l; i++)
		delete platforms[i];

	platforms.clear();
}

void Engine::resetPlayers()
{
	size_t l = players.size();

	for(size_t i = 0; i < l; i++) {
		players[i]->setX(START_X);
		players[i]->setY(START_Y);
	}

	Network::sendData();
}

bool Engine::loadMap(const std::string& path)
{
        std::string p = std::string(MAP_DIR) + path + ".map";
        std::ifstream infile(p.c_str());

        if(!infile.is_open()) {
                printf("Could not load map at path %s\n", p.c_str());
                return false;
        }

	clearPlatforms();
        resetPlayers();

        std::string line;
        while (std::getline(infile, line))
        {
                std::istringstream iss(line);
                double x, y, w, h;

                if (!(iss >> x >> y >> w >> h))
                        break;

                printf("%f %f %f %f read\n", x, y, w, h);

                platforms.push_back(new Platform(x, y, w, h));
        }

        return true;
}
