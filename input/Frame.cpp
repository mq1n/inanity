#include "Frame.hpp"
#include "../Exception.hpp"
#include <cstring>

BEGIN_INANITY_INPUT

Frame::Frame()
{
	Reset();
	memset(&state, 0, sizeof(state));
}

const Event& Frame::GetCurrentEvent() const
{
	if(nextEvent > 0 && nextEvent <= events.size())
		return events[nextEvent - 1];
	THROW("There is no current event");
}

const State& Frame::GetCurrentState() const
{
	return state;
}

bool Frame::NextEvent()
{
	// search for event which changes something
	while(nextEvent < events.size())
	{
		// process next event in state
		const Event& e = events[nextEvent++];

		switch(e.device)
		{
		case Event::deviceKeyboard:
			switch(e.keyboard.type)
			{
			case Event::Keyboard::typeKeyDown:
				// if key already pressed, skip
				if(state.keyboard[e.keyboard.key])
					continue;
				state.keyboard[e.keyboard.key] = 1;
				break;
			case Event::Keyboard::typeKeyUp:
				// if key already released, skip
				if(!state.keyboard[e.keyboard.key])
					continue;
				state.keyboard[e.keyboard.key] = 0;
				break;
			case Event::Keyboard::typeCharacter:
				break;
			}
			ProcessKeyboardVirtualEvents(e);
			break;
		case Event::deviceMouse:
			switch(e.mouse.type)
			{
			case Event::Mouse::typeButtonDown:
				// if mouse button is already pressed, skip
				if(state.mouseButtons[e.mouse.button])
					continue;
				state.mouseButtons[e.mouse.button] = true;
				break;
			case Event::Mouse::typeButtonUp:
				// if mouse button is already released, skip
				if(!state.mouseButtons[e.mouse.button])
					continue;
				state.mouseButtons[e.mouse.button] = false;
				break;
			case Event::Mouse::typeRawMove:
				// there is no state for raw move
				break;
			case Event::Mouse::typeCursorMove:
				state.cursorX = e.mouse.cursorX;
				state.cursorY = e.mouse.cursorY;
				break;
			case Event::Mouse::typeDoubleClick:
				break;
			}
			break;
		case Event::deviceController:
			// TODO: controller state
			break;
		}

		// if we are here, event changes something
		return true;
	}

	// if we are here, there are no events more
	return false;
}

void Frame::ForwardEvents()
{
	while(NextEvent());
}

void Frame::Reset()
{
	events.clear();
	nextEvent = 0;
}

void Frame::AddEvent(const Event& e)
{
	events.push_back(e);
}

void Frame::CopyTo(ptr<Frame> frame)
{
	frame->events = events;
	frame->nextEvent = nextEvent;
	frame->state = state;
}

void Frame::ProcessKeyboardVirtualEvents(const Event& e)
{
	switch(e.keyboard.type)
	{
	case Event::Keyboard::typeKeyDown:
	case Event::Keyboard::typeKeyUp:
		break;
	default:
		return;
	}

	Key virtualKey;
	bool newPressed;
	switch(e.keyboard.key)
	{
	case Key::ShiftL:
	case Key::ShiftR:
		virtualKey = Key::Shift;
		newPressed = state.keyboard[Key::ShiftL] || state.keyboard[Key::ShiftR];
		break;
	case Key::ControlL:
	case Key::ControlR:
		virtualKey = Key::Control;
		newPressed = state.keyboard[Key::ControlL] || state.keyboard[Key::ControlR];
		break;
	case Key::AltL:
	case Key::AltR:
		virtualKey = Key::Alt;
		newPressed = state.keyboard[Key::AltL] || state.keyboard[Key::AltR];
		break;
	default:
		return;
	}

	bool oldPressed = !!state.keyboard[virtualKey];

	if(newPressed != oldPressed)
	{
		Event virtualEvent;
		virtualEvent.device = Event::deviceKeyboard;
		virtualEvent.keyboard.type = newPressed ? Event::Keyboard::typeKeyDown : Event::Keyboard::typeKeyUp;
		virtualEvent.keyboard.key = virtualKey;
		events.insert(events.begin() + nextEvent, virtualEvent);
	}
}

END_INANITY_INPUT
