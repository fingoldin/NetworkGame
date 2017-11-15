#pragma once

#include <irrlicht/irrlicht.h>

#include "defines.hh"

class EventReceiver : public irr::IEventReceiver, public irr::IReferenceCounted
{
public:

	bool OnEvent(const irr::SEvent& event);

	bool keyDown(irr::EKEY_CODE code);

	bool mouseDown(E_MOUSE_BUTTON but);

	bool inputDown(E_INPUT input);

	EventReceiver(class Core *core);

	~EventReceiver() { }

private:

	void handleEvent(irr::EKEY_CODE key, bool prev, bool now);

	bool keys[irr::KEY_KEY_CODES_COUNT];
	bool mouseData[EMB_COUNT];

	class Core *core;

	bool connect();

        irr::gui::IGUIEditBox *edit_box;
	irr::gui::IGUIEnvironment *env;
};

