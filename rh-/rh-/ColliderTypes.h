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

//class ColliderSphere : public ColliderBase
//{
//public:
//	ColliderSphere(XMFLOAT3 center, float radius) : ColliderBase(Sphere)
//	{
//		_bounding.Center = center;
//		_bounding.Radius = radius;
//	};
//
//	ColliderSphere(Vector3 positionOffset, XMFLOAT3 center, float radius) : ColliderBase(Sphere)
//	{
//		_bounding.Center = center;
//		_bounding.Radius = radius;
//		_positionOffset = positionOffset;
//	};
//
//	~ColliderSphere() {};
//
//	BoundingSphere GetBounding()
//	{
//		return _bounding;
//	}
//
//	Vector3 GetCenter()
//	{
//		return _bounding.Center;
//	}
//
//	Vector3 GetPositionOffset()
//	{
//		return _positionOffset;
//	}
//
//	float GetRadius()
//	{
//		return _bounding.Radius;
//	}
//
//	void SetBounding(BoundingSphere bounding)
//	{
//		_bounding = bounding;
//	}
//
//	void SetCenter(Vector3 center)
//	{
//		_bounding.Center = center;
//	}
//
//	void SetPositionOffset(Vector3 offset)
//	{
//		_positionOffset = offset;
//	}
//
//	void SetRadius(float radius)
//	{
//		_bounding.Radius = radius;
//	}
//
//private:
//	BoundingSphere _bounding;
//	Vector3 _positionOffset;
//};

class ColliderAABB : public ColliderBase
{
public:
	ColliderAABB(XMFLOAT3 center, XMFLOAT3 extents) : ColliderBase(AABB)
	{
		_bounding.Center = center;
		_bounding.Extents = extents;
		UpdateMinMax();
	};

	ColliderAABB(Vector3 positionOffset, XMFLOAT3 center, XMFLOAT3 extents) : ColliderBase(AABB)
	{
		_bounding.Center = center;
		_bounding.Extents = extents;
		_positionOffset = positionOffset;
		UpdateMinMax();
	};

	ColliderAABB(Vector3 min, Vector3 max, bool flagMinMax) : ColliderBase(AABB), _min(min), _max(max)
	{
		UpdateCenterExtents();
	};

	ColliderAABB(Vector3 positionOffset, Vector3 min, Vector3 max, bool flagMinMax) : ColliderBase(AABB), _min(min), _max(max)
	{
		_positionOffset = positionOffset;
		UpdateCenterExtents();
	};

	~ColliderAABB() {};

	BoundingBox GetBounding()
	{
		return _bounding;
	}

	Vector3 GetCenter()
	{
		return _bounding.Center;
	}

	Vector3 GetExtents()
	{
		return _bounding.Extents;
	}

	Vector3 GetMin()
	{
		return _min;
	}

	Vector3 GetMax()
	{
		return _max;
	}

	Vector3 GetPositionOffset()
	{
		return _positionOffset;
	}

	void SetBounding(BoundingBox bounding)
	{
		_bounding = bounding;
		UpdateMinMax();
	}

	void SetBounding(XMFLOAT3 center, XMFLOAT3 extents)
	{
		_bounding.Center = center;
		_bounding.Extents = extents;
		UpdateMinMax();
	}

	void SetBounding(Vector3 min, Vector3 max, bool flagMinMax)
	{
		_min = min;
		_max = max;

		UpdateCenterExtents();
	}

	void SetCenter(Vector3 center)
	{
		_bounding.Center = center;
		UpdateMinMax();
	}

	void SetPositionOffset(Vector3 offset)
	{
		_positionOffset = offset;
	}

	void SetExtents(Vector3 extents)
	{
		_bounding.Extents = extents;
		UpdateMinMax();
	}

	void SetMin(Vector3 min)
	{
		_min = min;
		UpdateCenterExtents();
	}

	void SetMax(Vector3 max)
	{
		_max = max;
		UpdateCenterExtents();
	}

	void UpdateCenterExtents()
	{
		Vector3 half = Vector3(_max - _min) / 2.0f;
		Vector3 center = _min + half;

		_bounding.Center = center;
		_bounding.Extents = half;
	}

	void UpdateMinMax()
	{
		_min = Vector3(_bounding.Center - _bounding.Extents);
		_max = Vector3(_bounding.Center + _bounding.Extents);
	}

	void CalculateDimBounding()
	{
		Vector3 dimensions = _max - _min;

		if (dimensions == Vector3::Zero)
		{
			UpdateMinMax();
		}
		else
			if (Vector3(_bounding.Center) == Vector3::Zero || Vector3(_bounding.Extents) == Vector3::Zero)
			{
				Vector3 half = Vector3(_max - _min) / 2.0f;
				Vector3 center = _min + half;

				_bounding.Center = center;
				_bounding.Extents = half;
			}
	};

	void ChangeToCube()
	{
		Vector3 dimensions = _max - _min;

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
		_min = _max = Vector3::Zero;
		_bounding.Extents = XMFLOAT3(max, max, max);
		CalculateDimBounding();
	};

private:
	BoundingBox _bounding;
	Vector3 _min, _max;
	Vector3 _positionOffset;
};

class ColliderFrustum : public ColliderBase
{
public:
	ColliderFrustum(XMMATRIX xmProj) : ColliderBase(Frustum)
	{
		BoundingFrustum::CreateFromMatrix(_bounding, xmProj);
	};

	ColliderFrustum(BoundingFrustum bounding) : ColliderBase(Frustum)
	{
		_bounding = bounding;
	};

	~ColliderFrustum() {};

	BoundingFrustum GetBounding()
	{
		return _bounding;
	}

	void SetBounding(BoundingFrustum bounding)
	{
		_bounding = bounding;
	}

	XMFLOAT3 GetOrigin()
	{
		return _bounding.Origin;
	}

	void SetOrigin(XMFLOAT3 origin)
	{
		_bounding.Origin = origin;
	}

	XMFLOAT4 GetOrientation()
	{
		return _bounding.Orientation;
	}

	void SetOrientation(XMFLOAT4 orientation)
	{
		_bounding.Orientation = orientation;
	}

	void Transform(FXMMATRIX matrix)
	{
		XMVECTOR det;
		XMMATRIX InvViewMatrixLH = XMMatrixInverse(&det, matrix);
		_bounding.Transform(_bounding, InvViewMatrixLH);
	}

private:
	BoundingFrustum _bounding;
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

