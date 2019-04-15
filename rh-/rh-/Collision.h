#pragma once

#include <DirectXCollision.h>
#include "Entity.h"

using namespace DirectX;

typedef std::shared_ptr<Entity> EntityPtr;

struct ColliderSphere
{
	BoundingSphere Bounding;
	ContainmentType collision;
};

struct ColliderAABB
{
	BoundingBox Bounding;
	ContainmentType CollisionKind = DISJOINT;
};

struct ColliderOrientedBox
{
	BoundingOrientedBox Bounding;
	ContainmentType CollisionKind = DISJOINT;
};

struct ColliderRay
{
	XMVECTOR Origin;
	XMVECTOR Direction;
};

class Collision
{
public:
	EntityPtr OriginComponent;
	EntityPtr ColliderComponent;
	ContainmentType CollisionKind;

	Collision(EntityPtr origin, EntityPtr collider, ContainmentType collisionKind);
	~Collision();

	static XMVECTORF32 Collision::GetCollisionColor(ContainmentType collisionKind);
};

