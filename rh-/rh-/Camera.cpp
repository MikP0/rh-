//
// Camera.cpp
//
#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
	pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	posVector = XMLoadFloat3(&pos);
	rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotVector = XMLoadFloat3(&rot);
	UpdateViewMatrix();
	pitch = 0.0f;
	yaw = 0.0f;
}

Camera::~Camera()
{
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZplane, float farZplane)
{
	this->nearZ = nearZplane;
	this->farZ = farZplane;
	this->projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fovDegrees, aspectRatio, nearZ, farZ);
	this->projectionLhMatrix = XMMatrixPerspectiveFovLH(fovDegrees, aspectRatio, nearZ, farZ);
}

const XMMATRIX & Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX & Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

const XMMATRIX Camera::GetProjectionLhMatrix() const
{
	return this->projectionLhMatrix;
}

const XMMATRIX Camera::GetViewLhMatrix() const
{
	return viewLhMatrix;
}

const XMVECTOR & Camera::GetPositionVector() const
{
	return this->posVector;
}

const XMFLOAT3 & Camera::GetPositionFloat3() const
{
	return this->pos;
}

const XMVECTOR & Camera::GetRotationVector() const
{
	return this->rotVector;
}

const XMFLOAT3 & Camera::GetRotationFloat3() const
{
	return this->rot;
}

const int Camera::GetScreenWidth() const
{
	return this->screenWidth;
}

const int Camera::GetScreenHeight() const
{
	return this->screenHeight;
}

const float Camera::GetPitch() const
{
	return pitch;
}

const float Camera::GetYaw() const
{
	return yaw;
}

const float Camera::GetNearZ() const
{
	return this->nearZ;
}

const float Camera::GetFarZ() const
{
	return this->farZ;
}

const XMFLOAT3 Camera::GetZoom() const
{
	return this->zoom;
}

void Camera::SetScreenWidth(int width)
{
	this->screenWidth = width;
}

void Camera::SetScreenHeight(int height)
{
	this->screenHeight = height;
}

void Camera::SetPitch(float newPitch)
{
	pitch = newPitch;
}

void Camera::SetYaw(float newYaw)
{
	yaw = newYaw;
}

void Camera::SetNearZ(float nearZplane)
{
	this->nearZ = nearZplane;
}

void Camera::SetFarZ(float farZplane)
{
	this->farZ = farZplane;
}

void Camera::SetZoom(XMFLOAT3 zoom)
{
	this->zoom = zoom;
}

void Camera::ZoomIn() {
	if (zoom.y != 2) {
		zoom.y += 0.5f;
		zoom.z += -0.5f;
	}
}

void Camera::ZoomOut() {
	if (zoom.y != 0) {
		zoom.y += -0.5f;
		zoom.z += 0.5f;
	}
}

void Camera::SetPosition(const XMVECTOR & pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
	this->UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const XMVECTOR & pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const XMVECTOR & rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const XMVECTOR & rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	this->rot.y += y;
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch += atan(lookAtPos.y / distance);
	}

	if (lookAtPos.x != 0.0f)
	{
		yaw += atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR & Camera::GetForwardVector()
{
	return this->vec_forward;
}

const XMVECTOR & Camera::GetRightVector()
{
	return this->vec_right;
}

const XMVECTOR & Camera::GetBackwardVector()
{
	return this->vec_backward;
}

const XMVECTOR & Camera::GetLeftVector()
{
	return this->vec_left;
}

const XMVECTOR & Camera::GetUpVector()
{
	return this->vec_up;
}

void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
{
	//Calculate camera rotation matrix
	//XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	//XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the camera's current position
	//camTarget += this->posVector;
	//Calculate up direction based on current rotation
	//XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//Rebuild view matrix
	//this->viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(this->posVector, camTarget, upDir);

	float y = sinf(pitch);
	float r = cosf(pitch);
	float z = r * cosf(yaw);
	float x = r * sinf(yaw);

	DirectX::SimpleMath::Vector3 meme = GetPositionVector();

	XMVECTOR lookAt = meme + DirectX::SimpleMath::Vector3(x, y, z);

	this->viewMatrix = XMMatrixLookAtRH(meme, lookAt, DirectX::SimpleMath::Vector3::Up);
	this->viewLhMatrix = XMMatrixLookAtLH(meme, lookAt, DirectX::SimpleMath::Vector3::Up);

	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
	this->vec_up = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, vecRotationMatrix);
}