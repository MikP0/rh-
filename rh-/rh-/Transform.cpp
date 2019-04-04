#include "pch.h"
#include "Transform.h"


Transform::Transform()
{
}


Transform::~Transform()
{
}

std::shared_ptr<Transform> Transform::translate(const dxmath::Vector3 & position)
{
	return setPosition(_position + position);
}

std::shared_ptr<Transform> Transform::scale(const dxmath::Vector3 & scale)
{
	return setScale(_scale * scale);
}

std::shared_ptr<Transform> Transform::scale(float & scale)
{
	return setScale(_scale * scale);
}

std::shared_ptr<Transform> Transform::rotate(const dxmath::Vector3 & axis, float angle)
{
	_rotation = dxmath::Quaternion::CreateFromAxisAngle(axis, angle);
	return shared_from_this();
}

dxmath::Vector3 Transform::getPosition(void) const
{
	return _position;
}

dxmath::Quaternion Transform::getRotation(void) const
{
	return _rotation;
}

dxmath::Vector3 Transform::getScale(void) const
{
	return _scale;
}

std::shared_ptr<Transform> Transform::setPosition(const dxmath::Vector3 & position)
{
	this->_position = position;
	return shared_from_this();
}

std::shared_ptr<Transform> Transform::setRotation(const dxmath::Quaternion & rotation)
{
	this->_rotation = rotation;
	return shared_from_this();
}

std::shared_ptr<Transform> Transform::setScale(const dxmath::Vector3 & scale)
{
	this->_scale = scale;
	return shared_from_this();
}
