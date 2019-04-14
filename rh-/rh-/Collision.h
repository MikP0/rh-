#pragma once

#include <DirectXCollision.h>
#include "Entity.h"

using namespace DirectX;

typedef std::shared_ptr<Entity> EntityPtr;

struct ColliderAABB
{
	ContainmentType CollisionKind = DISJOINT;
	DirectX::BoundingBox BoundingBox;
};

struct ColliderRay
{
	DirectX::XMVECTOR Origin;
	DirectX::XMVECTOR Direction;
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

