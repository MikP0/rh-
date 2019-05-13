#include "pch.h"
#include "ColliderTypes.h"

ColliderBase::ColliderBase() : Type(None), CollisionKind(DISJOINT) 
{
};

ColliderBase::ColliderBase(ColliderType type) : Type(type), CollisionKind(DISJOINT) 
{
};

ColliderBase::~ColliderBase() 
{
};

ColliderSphere::ColliderSphere() : ColliderBase(Sphere) 
{
};

ColliderSphere::ColliderSphere(XMFLOAT3 center, float radius) : ColliderBase(Sphere)
{
	Bounding.Center = center;
	Bounding.Radius = radius;
};

ColliderSphere::~ColliderSphere() 
{
};

ColliderAABB::ColliderAABB() : ColliderBase(AABB) 
{
};

ColliderAABB::ColliderAABB(XMFLOAT3 center, XMFLOAT3 extents) : ColliderBase(AABB)
{
	Bounding.Center = center;
	Bounding.Extents = extents;
	Min = Vector3(Bounding.Center - Bounding.Extents);
	Max = Vector3(Bounding.Center + Bounding.Extents);
};

ColliderAABB::ColliderAABB(Vector3 min, Vector3 max, bool flagMinMax) : ColliderBase(AABB), Min(min), Max(max)
{
	Vector3 half = Vector3(Max - Min) / 2.0f;
	Vector3 center = Min + half;

	Bounding.Center = center;
	Bounding.Extents = half;
};

ColliderAABB::~ColliderAABB() 
{
};

void ColliderAABB::CalculateBounding()
{
	Vector3 dimensions = Max - Min;

	if (dimensions == Vector3::Zero)
	{
		Min = Vector3(Bounding.Center - Bounding.Extents);
		Max = Vector3(Bounding.Center + Bounding.Extents);
	}
	else
		if (Vector3(Bounding.Center) == Vector3::Zero || Vector3(Bounding.Extents) == Vector3::Zero)
		{
			Vector3 half = Vector3(Max - Min) / 2.0f;
			Vector3 center = Min + half;

			Bounding.Center = center;
			Bounding.Extents = half;
		}
};

void ColliderAABB::ChangeToCube()
{
	Vector3 dimensions = Max - Min;

	float max = dimensions.x;

	if (dimensions.x > dimensions.y)
	{
		if (dimensions.x > dimensions.z)
			max = dimensions.x;
		else
			max = dimensions.z;
	}
	else
	{
		if (dimensions.y > dimensions.z)
			max = dimensions.y;
		else
			max = dimensions.z;
	}

	max /= 2.0f;
	Min = Max = Vector3::Zero;
	Bounding.Extents = XMFLOAT3(max, max, max);
	CalculateBounding();
};

ColliderFrustum::ColliderFrustum() : ColliderBase(Frustum)
{
};

ColliderFrustum::ColliderFrustum(XMMATRIX xmProj)
{
	BoundingFrustum::CreateFromMatrix(Bounding, xmProj);
};

ColliderRay::ColliderRay() : ColliderBase(ColliderType::Ray) 
{
};

ColliderRay::ColliderRay(XMVECTOR origin, XMVECTOR direction) : ColliderBase(ColliderType::Ray)
{
	Origin = origin;
	Direction = direction;
};


ColliderRay::~ColliderRay()
{
};