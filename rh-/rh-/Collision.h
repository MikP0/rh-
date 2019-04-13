#pragma once
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

	std::unique_ptr<CommonStates>                           g_States;
	std::unique_ptr<BasicEffect>                            g_BatchEffect;
	std::unique_ptr<PrimitiveBatch<VertexPositionColor>>    g_Batch;

	Collision(EntityPtr origin, EntityPtr collider, ContainmentType collisionKind);
	~Collision();

	XMVECTOR Collision::GetCollisionColor(ContainmentType collisionKind);
};

