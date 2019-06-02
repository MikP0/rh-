#pragma once
#include "pch.h"

class Keyframe
{
	friend class BoneAnimation;

public:
	float Time() const;
	const DirectX::XMFLOAT3& Translation() const;
	const DirectX::XMFLOAT4& RotationQuaternion() const;
	const DirectX::XMFLOAT3& Scale() const;

	DirectX::XMVECTOR TranslationVector() const;
	DirectX::XMVECTOR RotationQuaternionVector() const;
	DirectX::XMVECTOR ScaleVector() const;

	DirectX::XMMATRIX Transform() const;

	float mTime;

private:
	Keyframe(float time, const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT4& rotationQuaternion, const DirectX::XMFLOAT3& scale);

	Keyframe();
	Keyframe(const Keyframe& rhs);
	Keyframe& operator=(const Keyframe& rhs);

	
	DirectX::XMFLOAT3 mTranslation;
	DirectX::XMFLOAT4 mRotationQuaternion;
	DirectX::XMFLOAT3 mScale;
};
