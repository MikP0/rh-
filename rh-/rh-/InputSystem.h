#pragma once

#include <vector>
#include <string>
#include "System.h"
#include "InputComponent.h"

typedef std::shared_ptr<InputComponent> InputComponentPtr;

class InputSystem : public System
{
public:
	InputSystem();
	~InputSystem();

	DirectX::Keyboard::State GetKeyboardState();
	DirectX::Mouse::State GetMouseState();

	std::map<availableKeys, actionList> GetActionKeysBindings(InputComponentPtr inputComponent);
	std::map<actionList, availableKeys> GetPushedBindedKeys(InputComponentPtr inputComponent);
	std::map<availableKeys, bool> GetDirectXpushedKeys();
	void SetWindow(HWND window);
	void SetMouseMode(DirectX::Mouse::Mode mode);

	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

	void InsertComponent(InputComponentPtr component); // temporary function for tests


protected:
	virtual void Iterate() override;

private:
	std::unique_ptr<DirectX::Keyboard> _keyboard;
	std::unique_ptr<DirectX::Mouse> _mouse;
	//std::map<availableKeys, DirectX::Mouse::State> _directXMouseKeysBinding;
	//std::map<availableKeys, DirectX::Keyboard::State> _directXKeyboardKeysBinding;
	//std::map<availableKeys, bool> _directXKeysBinding;
	ComponentType _componentsType;
	std::vector<InputComponentPtr> _components;
};

