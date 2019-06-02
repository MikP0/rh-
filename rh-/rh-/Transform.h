//
// Transform.h
//

#pragma once
#include "pch.h"

namespace dxmath = DirectX::SimpleMath;

class Transform : public std::enable_shared_from_this<Transform>
{
public:
	Transform(void);

	virtual ~Transform();

	std::shared_ptr<Transform> Translate(const dxmath::Vector3 &position, float time);
	std::shared_ptr<Transform> Scale(const dxmath::Vector3 &scale);
	std::shared_ptr<Transform> Scale(float &scale);
	std::shared_ptr<Transform> Rotate(const dxmath::Vector3 &axis, float angle);

	dxmath::Vector3 GetPosition(void) const;
	dxmath::Quaternion GetRotation(void) const;
	dxmath::Vector3 GetScale(void) const;
	bool GetUpdatedMoveFlag(void) const;

	dxmath::Matrix GetTransformMatrix(void) const;

	std::shared_ptr<Transform> SetPosition(const dxmath::Vector3 position);
	std::shared_ptr<Transform> SetRotation(const dxmath::Quaternion rotation);
	std::shared_ptr<Transform> SetScale(const dxmath::Vector3 scale);

	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);

	void SetUpdatedMoveFlag(bool state);
	
private:
	dxmath::Vector3 _position;
	dxmath::Quaternion _rotation;
	dxmath::Vector3 _scale;
	dxmath::Vector3 _localRotation;
	bool _updatedMoveFlag;
};