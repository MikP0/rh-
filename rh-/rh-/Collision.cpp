#include "pch.h"
#include "Collision.h"


Collision::Collision(PhysicsComponentPtr originComponent, PhysicsComponentPtr colliderComponent)
{
	OriginComponent = originComponent;
	ColliderComponent = colliderComponent;
}

Collision::~Collision()
{
}
