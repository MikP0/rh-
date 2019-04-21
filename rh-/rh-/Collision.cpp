#include "pch.h"
#include "Collision.h"


Collision::Collision(EntityPtr origin, EntityPtr collider, ContainmentType collisionKind)
{
	OriginComponent = origin;
	ColliderComponent = collider;
	CollisionKind = collisionKind;
}

Collision::Collision(EntityPtr origin, ContainmentType collisionKind, float rayIntersectDist)
{
	OriginComponent = origin;
	ColliderComponent = nullptr;
	CollisionKind = collisionKind;
	RayIntersectDist = rayIntersectDist;
}

Collision::~Collision()
{
}

XMVECTORF32 Collision::GetCollisionColor(DirectX::ContainmentType collisionKind)
{
	if (collisionKind == DirectX::DISJOINT)
		return DirectX::Colors::White;
	else
		if (collisionKind == DirectX::INTERSECTS)
			return DirectX::Colors::Yellow;
		else
			if (collisionKind == DirectX::CONTAINS)
				return DirectX::Colors::Red;
	return DirectX::Colors::Green;
}
