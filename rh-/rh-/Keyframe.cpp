#include "pch.h"
#include "Keyframe.h"
#include "VectorHelper.h"


Keyframe::Keyframe(float time, const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT4& rotationQuaternion, const DirectX::XMFLOAT3& scale)
	: mTime(time), mTranslation(translation), mRotationQuaternion(rotationQuaternion), mScale(scale)
{
}

float Keyframe::Time() const
{
	return mTime;
}

const DirectX::XMFLOAT3& Keyframe::Translation() const
{
	return mTranslation;
}

const DirectX::XMFLOAT4& Keyframe::RotationQuaternion() const
{
	return mRotationQuaternion;
}

const DirectX::XMFLOAT3& Keyframe::Scale() const
{
	return mScale;
}

DirectX::XMVECTOR Keyframe::TranslationVector() const
{
	return XMLoadFloat3(&mTranslation);
}

DirectX::XMVECTOR Keyframe::RotationQuaternionVector() const
{
	return XMLoadFloat4(&mRotationQuaternion);
}

DirectX::XMVECTOR Keyframe::ScaleVector() const
{
	return XMLoadFloat3(&mScale);
}

DirectX::XMMATRIX Keyframe::Transform() const
{
	static DirectX::XMVECTOR rotationOrigin = XMLoadFloat4(&Vector4Helper::Zero);

	return DirectX::XMMatrixAffineTransformation(ScaleVector(), rotationOrigin, RotationQuaternionVector(), TranslationVector());
}

