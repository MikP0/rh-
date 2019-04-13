#include "pch.h"
#include "PhysicsComponent.h"


PhysicsComponent::PhysicsComponent()
{
}

PhysicsComponent::~PhysicsComponent()
{
}

ComponentType PhysicsComponent::GetType(void)
{
	ComponentType componentType;
	componentType.name = "Physics";
	return componentType;
}
