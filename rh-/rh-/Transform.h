#pragma once

#include "SimpleMath.h"

namespace dxmath = DirectX::SimpleMath;

class Transform : public std::enable_shared_from_this<Transform>
{
public:
	Transform();
	virtual ~Transform();

	std::shared_ptr<Transform> translate(const dxmath::Vector3 &position);
	std::shared_ptr<Transform> scale(const dxmath::Vector3 &scale);
	std::shared_ptr<Transform> scale(float &scale);
	std::shared_ptr<Transform> rotate(const dxmath::Vector3 &axis, float angle);

	dxmath::Vector3 getPosition(void) const;
	dxmath::Quaternion getRotation(void) const;
	dxmath::Vector3 getScale(void) const;

	dxmath::Matrix getTransformMatrix(void) const;

	std::shared_ptr<Transform> setPosition(const dxmath::Vector3 &position);
	std::shared_ptr<Transform> setRotation(const dxmath::Quaternion &rotation);
	std::shared_ptr<Transform> setScale(const dxmath::Vector3 &scale);


private:
	dxmath::Vector3 _position;
	dxmath::Quaternion _rotation;
	dxmath::Vector3 _scale;

};