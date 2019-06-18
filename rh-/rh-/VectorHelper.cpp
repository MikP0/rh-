#include "pch.h"
#include <sstream>
#include "VectorHelper.h"


const DirectX::XMFLOAT2 Vector2Helper::Zero = DirectX::XMFLOAT2(0.0f, 0.0f);
const DirectX::XMFLOAT2 Vector2Helper::One = DirectX::XMFLOAT2(1.0f, 1.0f);

std::string Vector2Helper::ToString(const DirectX::XMFLOAT2& vector)
{
	std::stringstream stream;

	stream << "{" << vector.x << ", " << vector.y << "}";

	return stream.str();
}

const DirectX::XMFLOAT3 Vector3Helper::Zero = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
const DirectX::XMFLOAT3 Vector3Helper::One = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
const DirectX::XMFLOAT3 Vector3Helper::Forward = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);
const DirectX::XMFLOAT3 Vector3Helper::Backward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
const DirectX::XMFLOAT3 Vector3Helper::Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
const DirectX::XMFLOAT3 Vector3Helper::Down = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
const DirectX::XMFLOAT3 Vector3Helper::Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
const DirectX::XMFLOAT3 Vector3Helper::Left = DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f);

std::string Vector3Helper::ToString(const DirectX::XMFLOAT3& vector)
{
	std::stringstream stream;

	stream << "{" << vector.x << ", " << vector.y << ", " << vector.z << "}";

	return stream.str();
}

const DirectX::XMFLOAT4 Vector4Helper::Zero = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMFLOAT4 Vector4Helper::One = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

std::string Vector4Helper::ToString(const DirectX::XMFLOAT4& vector)
{
	std::stringstream stream;

	stream << "{" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << "}";

	return stream.str();
}

