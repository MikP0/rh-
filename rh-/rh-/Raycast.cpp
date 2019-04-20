#include "pch.h"
#include "Input.h"
#include "Raycast.h"

XMFLOAT3 Raycast::GetRayDirFromMousePos(int screenWidth, int screenHeight, int nearZ, int farZ, SimpleMath::Matrix view, SimpleMath::Matrix projection)
{
	XMVECTOR mouseNear = DirectX::XMVectorSet(Input::GetMousePosition().x, Input::GetMousePosition().y, 0.0f, 0.0f);
	XMVECTOR mouseFar = DirectX::XMVectorSet(Input::GetMousePosition().x, Input::GetMousePosition().y, 1.0f, 0.0f);
	XMVECTOR unprojectedNear = DirectX::XMVector3Unproject(mouseNear, 0, 0, screenWidth, screenHeight, nearZ, farZ,
		projection, view, XMMatrixIdentity());
	XMVECTOR unprojectedFar = DirectX::XMVector3Unproject(mouseFar, 0, 0, screenWidth, screenHeight, nearZ, farZ,
		projection, view, DirectX::XMMatrixIdentity());
	XMVECTOR result = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(unprojectedFar, unprojectedNear));
	XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, result);

	return direction;
}

XMFLOAT3 Raycast::GetPointOnGround(Camera camera, SimpleMath::Matrix view, SimpleMath::Matrix projection)
{
	// Get direction from mouse position
	XMFLOAT3 direction = GetRayDirFromMousePos(camera.GetScreenWidth(), camera.GetScreenHeight(), camera.GetNearZ(), camera.GetFarZ(), view, projection);

	//Get the distance to the ground.
	XMFLOAT3 cameraPosition = camera.GetPositionFloat3();

	//Get the point on the ground.
	cameraPosition.x += direction.x * (cameraPosition.y / -direction.y);
	cameraPosition.z += direction.z * (cameraPosition.y / -direction.y);

	return cameraPosition;
}

ColliderRay *Raycast::CastRay(XMVECTOR origin, XMVECTOR direction)
{
	return (new ColliderRay(origin, direction));
}
