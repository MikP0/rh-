#pragma once
#include "PhysicsComponent.h"

typedef std::shared_ptr<PhysicsComponent> PhysicsComponentPtr;

// Collision objects
struct CollisionSphere
{
	DirectX::BoundingSphere sphere;
	DirectX::ContainmentType collision;
};

struct CollisionBox
{
	DirectX::BoundingOrientedBox obox;
	DirectX::ContainmentType collision;
};

struct CollisionAABox
{
	DirectX::BoundingBox aabox;
	DirectX::ContainmentType collision;
};

struct CollisionFrustum
{
	DirectX::BoundingFrustum frustum;
	DirectX::ContainmentType collision;
};

struct CollisionTriangle
{
	DirectX::XMVECTOR pointa;
	DirectX::XMVECTOR pointb;
	DirectX::XMVECTOR pointc;
	DirectX::ContainmentType collision;
};

struct CollisionRay
{
	DirectX::XMVECTOR origin;
	DirectX::XMVECTOR direction;
};

class Collision
{
public:
	PhysicsComponentPtr OriginComponent;
	PhysicsComponentPtr ColliderComponent;

	Collision(PhysicsComponentPtr originComponent, PhysicsComponentPtr colliderComponent);
	~Collision();
};

