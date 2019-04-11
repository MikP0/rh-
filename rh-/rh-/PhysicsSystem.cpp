#include "pch.h"
#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem()
{
}


PhysicsSystem::~PhysicsSystem()
{
}

Collision PhysicsSystem::Collide(PhysicsComponentPtr collider1, PhysicsComponentPtr collider2)
{
	// to do
	return Collision(collider1, collider2);
}

std::vector<ComponentPtr> PhysicsSystem::GetComponents(ComponentType componentType)
{
	std::vector<ComponentPtr> allComponents;
	std::vector<ComponentPtr> selectedComponents;
	//allComponents = componentManager.GetAllComponents();

	for each (ComponentPtr component in allComponents)
	{
		if (std::dynamic_pointer_cast<PhysicsComponent>(component)->GetType().name.compare(componentType.name) == 0)
		{
			selectedComponents.push_back(component);
		}
	}

	return selectedComponents;
}

void PhysicsSystem::UpdateComponentsCollection()
{
	_components.clear();
	std::vector<ComponentPtr> selectedComponents = GetComponents(_componentsType);
	for each (ComponentPtr component in selectedComponents)
	{
		PhysicsComponentPtr physicsComponent = std::dynamic_pointer_cast<PhysicsComponent>(component);
		_components.push_back(physicsComponent);
	}
}

void PhysicsSystem::Iterate()
{
	for each (PhysicsComponentPtr component in _components)
	{

	}
}
