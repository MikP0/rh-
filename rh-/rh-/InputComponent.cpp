#include "pch.h"
#include "InputComponent.h"


InputComponent::InputComponent()
{
	SetIsEnabled(true);
}

InputComponent::InputComponent(std::map<availableKeys, actionList> actionSet)
{
	SetIsEnabled(true);
	SkillsKeys = actionSet;
}


InputComponent::~InputComponent()
{
}

ComponentType InputComponent::GetType(void)
{
	ComponentType componentType;
	componentType.name = "Input";
	return componentType;
}
