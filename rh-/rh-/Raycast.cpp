#include "pch.h"
#include "Input.h"
#include "Raycast.h"

XMFLOAT3 Raycast::GetRayDirFromMousePos(Camera camera)
{
	XMVECTOR mouseNear = DirectX::XMVectorSet(Input::GetMousePosition().x, Input::GetMousePosition().y, 0.0f, 0.0f);
	XMVECTOR mouseFar = DirectX::XMVectorSet(Input::GetMousePosition().x, Input::GetMousePosition().y, 1.0f, 0.0f);
	XMVECTOR unprojectedNear = DirectX::XMVector3Unproject(mouseNear, 0, 0, camera.GetScreenWidth(), camera.GetScreenHeight(), camera.GetNearZ(), camera.GetFarZ(),
		camera.GetProjectionMatrix(), camera.GetViewMatrix(), XMMatrixIdentity());
	XMVECTOR unprojectedFar = DirectX::XMVector3Unproject(mouseFar, 0, 0, camera.GetScreenWidth(), camera.GetScreenHeight(), camera.GetNearZ(), camera.GetFarZ(),
		camera.GetProjectionMatrix(), camera.GetViewMatrix(), XMMatrixIdentity());
	XMVECTOR result = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(unprojectedFar, unprojectedNear));
	XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, result);

	return direction;
}

XMFLOAT3 Raycast::GetPointOnGround(Camera camera)
{
	// Get direction from mouse position
	XMFLOAT3 direction = GetRayDirFromMousePos(camera);
	//XMFLOAT3 direction = GetRay(camera);
	//Get the distance to the ground.
	XMFLOAT3 cameraPosition = camera.GetPositionFloat3();

	//Get the point on the ground.
	cameraPosition.x += direction.x * (cameraPosition.y / -direction.y);
	cameraPosition.z += direction.z * (cameraPosition.y / -direction.y);
	cameraPosition.y = 0.0f;

	return cameraPosition;
}

XMFLOAT3 Raycast::GetRay(Camera camera) {
	float x = (2.0f * Input::GetMousePosition().x) / camera.GetScreenWidth() - 1.0f;
	float y = 1.0f - (2.0f * Input::GetMousePosition().y) / camera.GetScreenHeight();
	float z = 1.0f;
	/*XMFLOAT3 ray_nds = XMFLOAT3(x, y, z);
	XMVECTOR ray_clip = DirectX::XMVectorSet(ray_nds.x, ray_nds.y, -1.0f, 1.0f);

	XMMATRIX inverseproj = DirectX::XMMatrixInverse(nullptr, camera.GetProjectionMatrix());
	XMMATRIX inverseview = DirectX::XMMatrixInverse(nullptr, camera.GetViewMatrix());
	XMVECTOR ray_eye = DirectX::XMVector3Transform(ray_clip, inverseproj);
	XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, ray_eye);
	ray_eye = DirectX::XMVectorSet(temp.x, temp.y, -1.0f, 0.0f);
	XMVECTOR ray_wor = DirectX::XMVector3Transform(ray_eye, inverseview);
	XMVECTOR result = DirectX::XMVector3Normalize(ray_wor);
	XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, result);*/
	XMVECTOR mouseNear = DirectX::XMVectorSet(x, y, 0.0f, 1.0f);
	XMVECTOR mouseFar = DirectX::XMVectorSet(x, y, 1.0f, 1.0f);
	XMMATRIX matr = XMMatrixMultiply(camera.GetViewMatrix(), camera.GetProjectionMatrix());
		//camera.GetViewMatrix() * camera.GetProjectionMatrix();	
	XMMATRIX inverseviewproj = DirectX::XMMatrixInverse(nullptr, matr);
	XMVECTOR rayorigin = DirectX::XMVector3Transform(mouseNear, inverseviewproj);
	XMVECTOR rayend = DirectX::XMVector3Transform(mouseFar, inverseviewproj);
	XMVECTOR result = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayorigin, rayend));
	XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, result);

	return direction;
}

ColliderRay *Raycast::CastRay(XMVECTOR origin, XMVECTOR direction)
{
	return (new ColliderRay(origin, direction));
}

ColliderRay *Raycast::CastRay(Camera camera)
{
	XMFLOAT3 cameraPos = camera.GetPositionFloat3();
	XMVECTOR origin = Vector4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	XMFLOAT3 dirFromMouse = Raycast::GetRayDirFromMousePos(camera);
	XMVECTOR direction = Vector4(dirFromMouse.x, dirFromMouse.y, dirFromMouse.z, 0.0f);
	
	return CastRay(origin, direction);
}
