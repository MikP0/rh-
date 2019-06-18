//
// Transform.cpp
//
#include "pch.h"
#include "Transform.h"


Transform::Transform()
{
	_position = dxmath::Vector3::Zero;
	_scale = dxmath::Vector3::One;
	_rotation = dxmath::Quaternion::Identity;
	_localRotation = dxmath::Vector3::Zero;
	_updatedMoveFlag = false;
}

Transform::~Transform()
{
}

std::shared_ptr<Transform> Transform::Translate(const dxmath::Vector3 & position, float time)
{
	return SetPosition(_position + position);
}

std::shared_ptr<Transform> Transform::Scale(const dxmath::Vector3 & scale)
{
	return SetScale(_scale * scale);
}

std::shared_ptr<Transform> Transform::Scale(float & scale)
{
	return SetScale(_scale * scale);
}

std::shared_ptr<Transform> Transform::Rotate(const dxmath::Vector3 & axis, float angle)
{
	_rotation *= dxmath::Quaternion::CreateFromAxisAngle(axis, angle);
	return shared_from_this();
}

dxmath::Vector3 Transform::GetPosition(void) const
{
	return _position;
}

dxmath::Quaternion Transform::GetRotation(void) const
{
	return _rotation;
}

dxmath::Vector3 Transform::GetScale(void) const
{
	return _scale;
}

bool Transform::GetUpdatedMoveFlag() const
{
	return _updatedMoveFlag;
}

dxmath::Matrix Transform::GetTransformMatrix(void) const
{
	DirectX::XMVECTOR rotation = DirectX::XMVectorSet(_rotation.x, _rotation.y, _rotation.z, _rotation.w);
	DirectX::XMVECTOR rotOrigin = DirectX::XMVectorZero();
	return DirectX::XMMatrixAffineTransformation(_scale, rotOrigin, rotation, _position);
	//return dxmath::Matrix::CreateTranslation(_position) * dxmath::Matrix::CreateFromQuaternion(_rotation) * dxmath::Matrix::CreateScale(_scale);
}

std::shared_ptr<Transform> Transform::SetPosition(const dxmath::Vector3 position)
{
	_position = position;
	_updatedMoveFlag = true;
	return shared_from_this();
}

std::shared_ptr<Transform> Transform::SetRotation(const dxmath::Quaternion rotation)
{
	this->_rotation = rotation;
	return shared_from_this();
}

std::shared_ptr<Transform> Transform::SetScale(const dxmath::Vector3 scale)
{
	this->_scale = scale;
	return shared_from_this();
}

void Transform::SetX(float x)
{
	_position.x = x;
}

void Transform::SetY(float y)
{
	_position.y = y;
}

void Transform::SetZ(float z)
{
	_position.z = z;
}

void Transform::SetUpdatedMoveFlag(bool state)
{
	_updatedMoveFlag = state;
}