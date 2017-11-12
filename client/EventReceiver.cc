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
				Network::sendInputData(EI_LEFT, pressed);
				break;
			case irr::KEY_KEY_D:
				Network::sendInputData(EI_RIGHT, pressed);
				break;
			default:
				break;
			};
		}
	}
	else if(event.EventType == irr::EET_LOG_TEXT_EVENT) {
		printf("%s\n", event.LogEvent.Text);
	}

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
	default:
		return false;
	};
}

EventReceiver::EventReceiver(Core *core) : core(core)
{
	for(irr::u32 i = 0; i < irr::KEY_KEY_CODES_COUNT; i++)
		this->keys[i] = false;
}
