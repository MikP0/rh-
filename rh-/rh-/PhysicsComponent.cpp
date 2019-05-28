#include "pch.h"
#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent()
{
	isTrigger = false;
	ColliderBounding = make_shared<ColliderAABB>();
}

PhysicsComponent::PhysicsComponent(ColliderType colliderType)
{
	isTrigger = false;
	if (colliderType == AABB)
		ColliderBounding = make_shared<ColliderAABB>();
	else
		if (colliderType == Sphere)
			ColliderBounding = make_shared<ColliderSphere>();
		else
			ColliderBounding = make_shared<ColliderAABB>();
}

PhysicsComponent::PhysicsComponent(shared_ptr<ColliderBase> collider)
{
	isTrigger = false;
	ColliderBounding = collider;
}

PhysicsComponent::~PhysicsComponent()
{
}