#pragma once

#include <DirectXCollision.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

enum ColliderType
{
	None = 0,
	Sphere = 1,
	AABB = 2,
	OOBB = 3,
	Frustum = 4,
	Ray = 5
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

class ColliderFrustum : public ColliderBase
{
public:
	BoundingFrustum Bounding;

	ColliderFrustum() : ColliderBase(Frustum) {};
	ColliderFrustum(XMMATRIX xmProj)
	{
		BoundingFrustum::CreateFromMatrix(Bounding, xmProj);
	};
};

class ColliderRay : public ColliderBase
{
public:
	XMVECTOR Origin;
	XMVECTOR Direction;

	ColliderRay() : ColliderBase(ColliderType::Ray) {};

	ColliderRay(XMVECTOR origin, XMVECTOR direction) : ColliderBase(ColliderType::Ray)
	{
		Origin = origin;
		Direction = direction;
	};

	~ColliderRay() {};
};
