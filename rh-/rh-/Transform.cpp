//
// Transform.cpp
//

#include "Transform.h"


Transform::Transform()
{
	_position = dxmath::Vector3::Zero;
	_scale = dxmath::Vector3::One;
	_rotation = dxmath::Quaternion::Identity;
}


Transform::~Transform()
{
}

std::shared_ptr<Transform> Transform::Translate(const dxmath::Vector3 & position)
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
	//auto temp = dxmath::Quaternion::CreateFromAxisAngle(axis, angle);
	//temp *= _rotation;
	//_rotation = dxmath::Quaternion::Slerp(temp, _rotation, 0);


	_rotation = dxmath::Quaternion::CreateFromAxisAngle(axis, angle);
	return shared_from_this();
}

dxmath::Matrix Transform::RotateMy(const dxmath::Vector3 & axis, float angle)
{
	float a = angle;
	float c = cos(a);
	float s = sin(a);
	// v - axis   angle - angle   m - myworld
	dxmath::Vector3 axises = axis;
	axises.Normalize();
	dxmath::Vector3 temp = (float(1) - c) * axises;


	dxmath::Matrix Rotate;

	Rotate._11 = c + temp.x * axises.x;
	Rotate._12 = temp.x * axises.y + s * axises.z;
	Rotate._13 = temp.x * axises.z - s * axises.y;

	Rotate._21 = temp.y * axises.x - s * axises.z;
	Rotate._22 = c + temp.y * axis.y;
	Rotate._23 = temp.y * axises.z + s * axises.x;

	Rotate._31 = temp.z * axises.x + s * axises.y;
	Rotate._32 = temp.z * axises.y - s * axises.x;
	Rotate._33 = c + temp.z * axises.z;

	dxmath::Matrix myworld = GetTransformMatrix();


	dxmath::Vector4 res1 = dxmath::Vector3(myworld._11, myworld._12, myworld._13) * Rotate._11 + dxmath::Vector3(myworld._21, myworld._22, myworld._23) * Rotate._12 + dxmath::Vector3(myworld._31, myworld._32, myworld._33) * Rotate._13;
	dxmath::Vector4 res2 = dxmath::Vector3(myworld._11, myworld._12, myworld._13) * Rotate._21 + dxmath::Vector3(myworld._21, myworld._22, myworld._23) * Rotate._22 + dxmath::Vector3(myworld._31, myworld._32, myworld._33) * Rotate._23;
	dxmath::Vector4 res3 = dxmath::Vector3(myworld._11, myworld._12, myworld._13) * Rotate._31 + dxmath::Vector3(myworld._21, myworld._22, myworld._23) * Rotate._32 + dxmath::Vector3(myworld._31, myworld._32, myworld._33) * Rotate._33;
	dxmath::Vector4 res4 = dxmath::Vector3(myworld._41, myworld._42, myworld._43);


	dxmath::Matrix result(res1, res2, res3, res4);

	return result;
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

dxmath::Matrix Transform::GetTransformMatrix(void) const
{
	DirectX::XMVECTOR scale = _scale;
	DirectX::XMVECTOR translation = _position;

	//Quaternion Q = Quaternion(GetRotationF3());
	dxmath::Quaternion Q = _rotation;
	DirectX::XMVECTOR rotation = DirectX::XMVectorSet(Q.x, Q.y, Q.z, Q.w);
	//XMVECTOR rotOrigin = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR rotOrigin = DirectX::XMVectorZero();
	return DirectX::XMMatrixAffineTransformation(scale, rotOrigin, rotation, translation);

	//return dxmath::Matrix::CreateTranslation(_position) * dxmath::Matrix::CreateFromQuaternion(_rotation) * dxmath::Matrix::CreateScale(_scale);
}

std::shared_ptr<Transform> Transform::SetPosition(const dxmath::Vector3 position)
{
	_position = position;
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

dxmath::Vector3 QuaternionTransformVector(const dxmath::Vector3 & v, dxmath::Quaternion quart);
dxmath::Quaternion Conjugate(dxmath::Quaternion quart);

void Transform::RotateAroundPointAndAxis(const dxmath::Vector3& axis, float angle, const dxmath::Vector3& point)
{
	dxmath::Vector3 R(_position - point);
	const dxmath::Quaternion rot = dxmath::Quaternion::CreateFromAxisAngle(axis, angle);
	R = QuaternionTransformVector(R, rot);
	_position = point + R;
}

dxmath::Vector3 QuaternionTransformVector(const dxmath::Vector3 & v, dxmath::Quaternion quart)
{
	dxmath::Quaternion pure(v, 0.0f);
	dxmath::Quaternion rotated = quart * pure * Conjugate(quart);
	return dxmath::Vector3(rotated.x, rotated.y, rotated.z);
}

dxmath::Quaternion Conjugate(dxmath::Quaternion quart)
{
	dxmath::Quaternion result;
	result.w = quart.w;
	result.x = -quart.x;
	result.y = -quart.y;
	result.z = -quart.z;
	return result;
}