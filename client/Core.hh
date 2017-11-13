#pragma once

#include <irrlicht/irrlicht.h>

#include <string>

#include "EventReceiver.hh"
#include "NodeManager.hh"
#include "Network.hh"
#include "Camera.hh"

class Core : public irr::IReferenceCounted
{
public:
	Core();
	~Core() { }

	void begin(const char * winName);

	void end(void);

	bool run(void);

	bool active(void);

	void yield(void);

	void update(void);

	void render(void);

	inline NodeManager *getNodeManager() const { return node_manager; }
	inline EventReceiver *getEventReceiver() const { return event_receiver; }
	inline irr::gui::IGUIEnvironment *getGUIEnvironment() const { return device ? device->getGUIEnvironment() : NULL; }

private:

	void build_keyboard(int x, int y, float s);

	void init_device(const char * winName);

	irr::core::stringc win_name;

	irr::IrrlichtDevice * device;
	irr::video::IVideoDriver * driver;
	irr::gui::IGUIEnvironment * gui_env;
	EventReceiver * event_receiver;
	NodeManager *node_manager;

	Camera *camera;

	irr::SIrrlichtCreationParameters params;
};
