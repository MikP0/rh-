#pragma once

#include "Camera.h"
#include "Collision.h"

using namespace DirectX;

class Raycast
{
public:
	static XMFLOAT3 GetRayDirFromMousePos(Camera camera);
	static XMFLOAT3 GetPointOnGround(Camera camera);
	static ColliderRay *CastRay(XMVECTOR origin, XMVECTOR direction);
	static ColliderRay *CastRay(Camera camera);
	static XMFLOAT3 GetRay(Camera camera);

private:
	Raycast() {};
	~Raycast() {};
};

