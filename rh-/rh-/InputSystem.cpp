#include "pch.h"
#include "InputSystem.h"


InputSystem::InputSystem()
{
	_componentsType.name = "Input";
	_keyboard = std::make_unique<DirectX::Keyboard>();
	_mouse = std::make_unique<DirectX::Mouse>();
	
}

InputSystem::~InputSystem()
{
}

DirectX::Keyboard::State InputSystem::GetKeyboardState()
{
	return _keyboard->GetState();
}

DirectX::Mouse::State InputSystem::GetMouseState()
{
	return _mouse->GetState();
}

std::map<availableKeys, actionList> InputSystem::GetActionKeysBindings(InputComponentPtr inputComponent)
{
	return inputComponent->SkillsKeys;
}

std::map<availableKeys, bool> InputSystem::GetPushedKeys()
{
	DirectX::Mouse::State MouseState = GetMouseState();
	DirectX::Keyboard::State KeyboardState = GetKeyboardState();

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

void InputSystem::SetWindow(HWND window)
{
	_mouse->SetWindow(window);
}

void InputSystem::SetMouseMode(DirectX::Mouse::Mode mode)
{
	_mouse->SetMode(mode);
}

std::vector<actionList> InputSystem::GetActions(InputComponentPtr inputComponent)
{
	std::vector<actionList> pushedBindedKeys;
	std::map<availableKeys, actionList> actionKeyBindings = GetActionKeysBindings(inputComponent);
	std::map<availableKeys, bool> pushedKeys = GetPushedKeys();


	for (std::map<availableKeys, bool>::iterator iter = pushedKeys.begin(); iter != pushedKeys.end(); ++iter)
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

std::vector<ComponentPtr> InputSystem::GetComponents(ComponentType componentType)
{
	std::vector<ComponentPtr> allComponents;
	std::vector<ComponentPtr> selectedComponents;
	//allComponents = componentManager.GetAllComponents();

	for each (ComponentPtr component in allComponents)
	{
		if (std::dynamic_pointer_cast<InputComponent>(component)->GetType().name.compare(componentType.name) == 0)
		{
			selectedComponents.push_back(component);
		}
	}

	return selectedComponents;
}

void InputSystem::UpdateComponentsCollection()
{
	_components.clear();
	std::vector<ComponentPtr> selectedComponents = GetComponents(_componentsType);
	for each (ComponentPtr component in selectedComponents)
	{
		InputComponentPtr inputComponent = std::dynamic_pointer_cast<InputComponent>(component);
		_components.push_back(inputComponent);
	}
}

void InputSystem::InsertComponent(InputComponentPtr component)
{
	_components.push_back(component);
}

void InputSystem::Iterate()
{
	for each (InputComponentPtr component in _components)
	{

	}
}

