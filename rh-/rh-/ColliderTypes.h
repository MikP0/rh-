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
	ColliderType Type;
	ContainmentType CollisionKind;

	ColliderBase();
	ColliderBase(ColliderType type);
	virtual ~ColliderBase();
};

class ColliderSphere : public ColliderBase
{
public:
	BoundingSphere Bounding;

	ColliderSphere();
	ColliderSphere(XMFLOAT3 center, float radius);
	~ColliderSphere();
};

class ColliderAABB : public ColliderBase
{
public:
	BoundingBox Bounding;
	Vector3 Min, Max;

	ColliderAABB();

	ColliderAABB(XMFLOAT3 center, XMFLOAT3 extents);
	ColliderAABB(Vector3 min, Vector3 max, bool flagMinMax);
	~ColliderAABB();

	void CalculateBounding();
	void ChangeToCube();
};

class ColliderFrustum : public ColliderBase
{
public:
	BoundingFrustum Bounding;

	ColliderFrustum();
	ColliderFrustum(XMMATRIX xmProj);
};

class ColliderRay : public ColliderBase
{
public:
	XMVECTOR Origin;
	XMVECTOR Direction;

	ColliderRay();
	ColliderRay(XMVECTOR origin, XMVECTOR direction);
	~ColliderRay();
};

