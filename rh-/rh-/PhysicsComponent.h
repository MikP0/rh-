#pragma once
#include "Component.h"
#include "Collision.h"

template<typename T>
class PhysicsComponent : public Component
{
public:
	T ColliderBounding;

	PhysicsComponent() {};
	~PhysicsComponent() {};

	virtual ComponentType GetType(void) override
	{
		ComponentType componentType;
		componentType.name = "Physics";
		return componentType;
	}
};

