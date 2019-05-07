#pragma once

#include <DirectXCollision.h>
#include "Entity.h"

using namespace DirectX;

typedef std::shared_ptr<Entity> EntityPtr;

enum ColliderType
{
	None = 0,
	Sphere = 1,
	AABB = 2,
	OOBB = 3,
	Ray = 4
};

class ColliderBase
{
public:
	ColliderBase() : Type(None), CollisionKind(DISJOINT) {};
	ColliderBase(ColliderType type) : Type(type), CollisionKind(DISJOINT) {};
	virtual ~ColliderBase() {};

	ColliderType Type;
	ContainmentType CollisionKind;
};

class ColliderSphere : public ColliderBase
{
public:
	BoundingSphere Bounding;

	ColliderSphere() : ColliderBase(Sphere) {};

	ColliderSphere(XMFLOAT3 center, float radius) : ColliderBase(Sphere) 
	{
		Bounding.Center = center;
		Bounding.Radius = radius;
	};

	~ColliderSphere() {};
};

class ColliderAABB : public ColliderBase
{
public:
	BoundingBox Bounding;
	Vector3 Min, Max;

	ColliderAABB() : ColliderBase(AABB) {};

	ColliderAABB(XMFLOAT3 center, XMFLOAT3 extents) : ColliderBase(AABB)
	{
		Bounding.Center = center;
		Bounding.Extents = extents;
		Min = Vector3(Bounding.Center - Bounding.Extents);
		Max = Vector3(Bounding.Center + Bounding.Extents);
	};

	ColliderAABB(Vector3 min, Vector3 max, bool flagMinMax) : ColliderBase(AABB), Min(min), Max(max)
	{
		Vector3 half = Vector3(Max - Min) / 2.0f;
		Vector3 center = Min + half;

		Bounding.Center = center;
		Bounding.Extents = half;
	};

	~ColliderAABB() {};
};

class ColliderRay
{
public:
	ColliderType Type;
	XMVECTOR Origin;
	XMVECTOR Direction;

	ColliderRay() : Type(ColliderType::Ray) {};

	ColliderRay(XMVECTOR origin, XMVECTOR direction) : Type(ColliderType::Ray)
	{
		Origin = origin;
		Direction = direction;
	};

	~ColliderRay() {};
};

class Collision
{
public:
	EntityPtr OriginComponent;
	EntityPtr ColliderComponent;
	ContainmentType CollisionKind;
	float RayIntersectDist;

	Collision(EntityPtr origin, EntityPtr collider, ContainmentType collisionKind);
	Collision(EntityPtr origin, ContainmentType collisionKind, float rayIntersectDist);
	~Collision();

	static XMVECTORF32 Collision::GetCollisionColor(ContainmentType collisionKind);
};

