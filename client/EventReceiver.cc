#include "EventReceiver.hh"

#include "Core.hh"

#include "Network.hh"

bool EventReceiver::OnEvent(const irr::SEvent& event)
{
	if(event.EventType == irr::EET_KEY_INPUT_EVENT) {
		bool prev = keys[event.KeyInput.Key];
		bool pressed = event.KeyInput.PressedDown;

		keys[event.KeyInput.Key] = pressed;

		if(prev != pressed) {
			switch(event.KeyInput.Key)
			{
			case irr::KEY_KEY_A:
			case irr::KEY_KEY_D:
			case irr::KEY_SPACE:
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
	return this->keys[code];
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
	default:
		return false;
	};
}

EventReceiver::EventReceiver(Core *core) : core(core), edit_box(NULL), env(NULL)
{
	for(irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; i++)
		this->keys[i] = false;

	env = core->getGUIEnvironment();

	if(env) {
                 edit_box = env->addEditBox(L"", irr::core::rect<irr::s32>(CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH, GUI_PAD, CORE_WINDOW_WIDTH - GUI_PAD, GUI_PAD + EDIT_BOX_HEIGHT), true, 0, EDIT_BOX_ID);

                 env->addButton(irr::core::rect<irr::s32>(CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD,
                                                      CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH / 2 - GUI_PAD / 2, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD + BUTTON_HEIGHT), 0, CONNECT_BOX_ID, L"Connect");

                 env->addButton(irr::core::rect<irr::s32>(CORE_WINDOW_WIDTH - GUI_PAD - GUI_WIDTH / 2 + GUI_PAD / 2, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD,
                                                      CORE_WINDOW_WIDTH - GUI_PAD, GUI_PAD + EDIT_BOX_HEIGHT + GUI_PAD + BUTTON_HEIGHT), 0, DISCONNECT_BOX_ID, L"Disconnect");
        }
}
