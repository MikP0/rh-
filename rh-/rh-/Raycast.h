#pragma once

#include "Camera.h"
#include "Collision.h"

using namespace DirectX;

class Raycast
{
public:
	static XMFLOAT3 GetRayDirFromMousePos(int screenWidth, int screenHeight, int nearZ, int farZ, SimpleMath::Matrix view, SimpleMath::Matrix projection);
	static XMFLOAT3 GetPointOnGround(Camera camera, SimpleMath::Matrix view, SimpleMath::Matrix projection);
	static ColliderRay *CastRay(XMVECTOR origin, XMVECTOR direction);

private:
	Raycast() {};
	~Raycast() {};
};

