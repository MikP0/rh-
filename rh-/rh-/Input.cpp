#include "pch.h"
#include "Input.h"

unique_ptr<Keyboard> Input::_keyboard(new Keyboard());
unique_ptr<Mouse> Input::_mouse(new Mouse());
map<availableKeys, actionList> Input::AvailableKeysActionsBinding = {};

Input::Input()
{
}


Input::~Input()
{
}

Keyboard::State Input::GetKeyboardState()
{
	return _keyboard->GetState();
}

Mouse::State Input::GetMouseState()
{
	return _mouse->GetState();
}

map<availableKeys, actionList> Input::GetActionKeysBindings()
{
	return AvailableKeysActionsBinding;
}

std::map<availableKeys, bool> Input::GetPushedKeys()
{
	Mouse::State MouseState = _mouse->GetState();
	Keyboard::State KeyboardState = _keyboard->GetState();

	std::map<availableKeys, bool> pushedKeys;

	if (MouseState.leftButton)
		pushedKeys[lpm] = true;

	if (KeyboardState.Escape)
		pushedKeys[esc] = true;

	if (KeyboardState.Space)
		pushedKeys[space] = true;

	if (KeyboardState.LeftControl)
		pushedKeys[leftControl] = true;

	if (KeyboardState.A)
		pushedKeys[a] = true;

	if (KeyboardState.D)
		pushedKeys[d] = true;

	if (KeyboardState.W)
		pushedKeys[w] = true;

	if (KeyboardState.S)
		pushedKeys[s] = true;

	return pushedKeys;
}

SimpleMath::Vector2 Input::GetMousePosition()
{
	Mouse::State MouseState = GetMouseState();

	return DirectX::SimpleMath::Vector2(MouseState.x, MouseState.y);
}

void Input::SetWindowForMouse(HWND window)
{
	_mouse->SetWindow(window);
}

void Input::SetMouseMode(Mouse::Mode mode)
{
	_mouse->SetMode(mode);
}

vector<actionList> Input::GetActions()
{
	vector<actionList> pushedBindedKeys;
	map<availableKeys, actionList> actionKeyBindings = AvailableKeysActionsBinding;
	map<availableKeys, bool> pushedKeys = GetPushedKeys();

	for (map<availableKeys, bool>::iterator iter = pushedKeys.begin(); iter != pushedKeys.end(); ++iter)
	{
		//Key k = iter->first;
		//Value v = iter->second;

		if (actionKeyBindings.find(iter->first) != actionKeyBindings.end() && iter->second)
		{
			pushedBindedKeys.push_back(actionKeyBindings[iter->first]);
		}
	}

	return pushedBindedKeys;
}
