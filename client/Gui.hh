#include <irrlicht/irrlicht.h>

#include "defines.hh"

class Gui : public irr::IEventReceiver, public irr::IReferenceCounted
{
public:
	Gui(irr::gui::IGUIEnvironment *env);

	bool OnEvent(const irr::SEvent& event);

private:
	bool connect();

	irr::gui::IGUIEnvironment *env;

	irr::gui::IGUIEditBox *edit_box;
};
