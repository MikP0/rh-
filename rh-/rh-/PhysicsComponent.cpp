#include "pch.h"
#include "PhysicsComponent.h"


PhysicsComponent::PhysicsComponent()
{
	ColliderType = AABB;
}

PhysicsComponent::PhysicsComponent(int colliderType)
{
	if (colliderType == 0)
		ColliderType = Sphere;
	else
		if (colliderType == 1)
			ColliderType = AABB;
		else
			ColliderType = OBB;
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
