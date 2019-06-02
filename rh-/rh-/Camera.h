//
// Camera.h
//

#pragma once
#include "pch.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void SetProjectionValues(float fovDegreesPlane, float aspectRatioPlane, float nearZplane, float farZplane);

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;
	const XMMATRIX GetProjectionLhMatrix() const;
	const XMMATRIX GetViewLhMatrix() const;

	const XMVECTOR & GetPositionVector() const;
	const XMFLOAT3 & GetPositionFloat3() const;
	const XMVECTOR & GetRotationVector() const;
	const XMFLOAT3 & GetRotationFloat3() const;

	const int GetScreenWidth() const;
	const int GetScreenHeight() const;
	const float GetPitch() const;
	const float GetYaw() const;
	const float GetNearZ() const;
	const float GetFarZ() const;
	const XMFLOAT3 GetZoom() const;

	void SetScreenWidth(int width);
	void SetScreenHeight(int height);
	void SetPitch(float newPitch);
	void SetYaw(float newYaw);
	void SetNearZ(float nearZplane);
	void SetFarZ(float farZplane);
	void SetZoom(XMFLOAT3 zoom);
	void ZoomIn();
	void ZoomOut();

	void SetPosition(const XMVECTOR & pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR & pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const XMVECTOR & rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR & rot);
	void AdjustRotation(float x, float y, float z);
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	const XMVECTOR & GetForwardVector();
	const XMVECTOR & GetRightVector();
	const XMVECTOR & GetBackwardVector();
	const XMVECTOR & GetLeftVector();
	const XMVECTOR & GetUpVector();
private:
	void UpdateViewMatrix();
	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMMATRIX viewMatrix, viewLhMatrix;
	XMMATRIX projectionMatrix, projectionLhMatrix;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
	XMVECTOR vec_up;

	int screenWidth, screenHeight;

	float nearZ, farZ;
	float pitch;
	float yaw;
	XMFLOAT3 zoom;
};