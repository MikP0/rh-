#include "pch.h"
#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent()
{
	ColliderBounding = std::make_shared<ColliderAABB>();
}

PhysicsComponent::PhysicsComponent(ColliderType colliderType)
{
	if (colliderType == AABB)
		ColliderBounding = std::make_shared<ColliderAABB>();
	else
		if (colliderType == Sphere)
			ColliderBounding = std::make_shared<ColliderSphere>();
		else
			ColliderBounding = std::make_shared<ColliderAABB>();
}

PhysicsComponent::~PhysicsComponent()
{
}

ComponentType PhysicsComponent::GetType(void)
{
	return ComponentType("Physics");
}