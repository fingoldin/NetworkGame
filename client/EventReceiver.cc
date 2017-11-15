#include "EventReceiver.hh"

#include "Core.hh"

#include "Network.hh"

#include "NodeManager.hh"
#include "Player.hh"

bool EventReceiver::OnEvent(const irr::SEvent& event)
{
	Player *lp = NULL;
        if(core) {
                NodeManager *m = core->getNodeManager();

                if(m)
                        lp = m->getLocalPlayer();
        }

	if(event.EventType == irr::EET_KEY_INPUT_EVENT) {
		bool prev = keys[event.KeyInput.Key];
		bool pressed = event.KeyInput.PressedDown;

		keys[event.KeyInput.Key] = pressed;

		if(prev != pressed) {
			switch(event.KeyInput.Key)
			{
			case irr::KEY_KEY_A:
				if(lp) lp->setInput(EI_LEFT, pressed);
				Network::sendInputData();
                                break;
			case irr::KEY_KEY_D:
				if(lp) lp->setInput(EI_RIGHT, pressed);
                                Network::sendInputData();
                                break;
			case irr::KEY_SPACE:
				if(lp) lp->setInput(EI_JUMP, pressed);
                                Network::sendInputData();
                                break;
			default:
				break;
			};
		}
	}
	else if(event.EventType == irr::EET_LOG_TEXT_EVENT) {
		printf("%s\n", event.LogEvent.Text);
	}
        else if(event.EventType == irr::EET_GUI_EVENT && env && edit_box)
        {
                irr::s32 id = event.GUIEvent.Caller->getID();

                switch(event.GUIEvent.EventType)
                {
                case irr::gui::EGET_EDITBOX_ENTER:
                        if(id == EDIT_BOX_ID) {
				env->setFocus(NULL);
                                return connect();
			}
                        else
                                break;
                case irr::gui::EGET_BUTTON_CLICKED:
                        if(id == CONNECT_BOX_ID)
                                return connect();
                        else if(id == DISCONNECT_BOX_ID) {
                                Network::disconnect();
                                return true;
                        }
                        else
                                break;
                default:
                        break;
                };
        }
	else if(event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
		if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN ||
		   event.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
			mouseData[EMB_LEFT] = event.MouseInput.isLeftPressed();
			if(lp) lp->setInput(EI_PUNCH, mouseData[EMB_LEFT]);
		}
	}

        return false;
}

bool EventReceiver::connect()
{
        if(!edit_box)
                return false;

        const wchar_t *text = edit_box->getText();

        std::string map = Network::connect(irr::core::stringc(text).c_str());

	NodeManager *m = core->getNodeManager();

	if(m && map != "")
		return m->loadMap(map);

        return false;
}

bool EventReceiver::keyDown(irr::EKEY_CODE code)
{
	return keys[code];
}

bool EventReceiver::mouseDown(E_MOUSE_BUTTON but)
{
	return mouseData[but];
}

bool EventReceiver::inputDown(E_INPUT input)
{
	switch(input)
	{
	case EI_LEFT:
		return keyDown(irr::KEY_KEY_A);
	case EI_RIGHT:
		return keyDown(irr::KEY_KEY_D);
	case EI_JUMP:
		return keyDown(irr::KEY_SPACE);
	case EI_PUNCH:
		return mouseDown(EMB_LEFT);
	default:
		return false;
	};
}

EventReceiver::EventReceiver(Core *core) : core(core), edit_box(NULL), env(NULL)
{
	for(irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; i++)
		keys[i] = false;

	for(int i = 0; i < EMB_COUNT; i++)
		mouseData[i] = false;

	env = core->getGUIEnvironment();

	if(env) {
                 edit_box = env->addEditBox(L"", irr::core::rect<irr::s32>(CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH, GUI_PAD, CORE_WINDOW_WIDTH - GUI_PAD, GUI_PAD + EDIT_BOX_HEIGHT), true, 0, EDIT_BOX_ID);

                 env->addButton(irr::core::rect<irr::s32>(CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD,
                                                      CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH / 2 - GUI_PAD / 2, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD + BUTTON_HEIGHT), 0, CONNECT_BOX_ID, L"Connect");

                 env->addButton(irr::core::rect<irr::s32>(CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH / 2 + GUI_PAD / 2, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD,
                                                      CORE_WINDOW_WIDTH - GUI_PAD, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD + BUTTON_HEIGHT), 0, DISCONNECT_BOX_ID, L"Disconnect");
        }
}
