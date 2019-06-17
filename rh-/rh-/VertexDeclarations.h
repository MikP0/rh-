#pragma once
#include "pch.h"

typedef struct _VertexPosition
{
	DirectX::XMFLOAT4 Position;

	_VertexPosition() { }

	_VertexPosition(const DirectX::XMFLOAT4& position)
		: Position(position) { }
} VertexPosition;

typedef struct _VertexPositionColorSkin
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT4 Color;

	_VertexPositionColorSkin() { }

	_VertexPositionColorSkin(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& color)
		: Position(position), Color(color) { }
} VertexPositionColorSkin;

typedef struct _VertexPositionTexture
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT2 TextureCoordinates;

	_VertexPositionTexture() { }

	_VertexPositionTexture(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates)
		: Position(position), TextureCoordinates(textureCoordinates) { }
} VertexPositionTexture;

typedef struct _VertexPositionNormal
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT3 Normal;

	_VertexPositionNormal() { }

	_VertexPositionNormal(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT3& normal)
		: Position(position), Normal(normal) { }
} VertexPositionNormal;

typedef struct _VertexPositionTextureNormal
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT2 TextureCoordinates;
	DirectX::XMFLOAT3 Normal;

	_VertexPositionTextureNormal() { }

	_VertexPositionTextureNormal(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates, const DirectX::XMFLOAT3& normal)
		: Position(position), TextureCoordinates(textureCoordinates), Normal(normal) { }
} VertexPositionTextureNormal;

typedef struct _VertexSkinnedPositionTextureNormal
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT2 TextureCoordinates;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMUINT4 BoneIndices;
	DirectX::XMFLOAT4 BoneWeights;

	_VertexSkinnedPositionTextureNormal() { }

	_VertexSkinnedPositionTextureNormal(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates, const DirectX::XMFLOAT3& normal, const DirectX::XMUINT4& boneIndices, const DirectX::XMFLOAT4& boneWeights)
		: Position(position), TextureCoordinates(textureCoordinates), Normal(normal), BoneIndices(boneIndices), BoneWeights(boneWeights) { }
} VertexSkinnedPositionTextureNormal;

typedef struct _VertexSkinnedPosition
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMUINT4 BoneIndices;
	DirectX::XMFLOAT4 BoneWeights;

	_VertexSkinnedPosition() { }

	_VertexSkinnedPosition(const DirectX::XMFLOAT4& position, const DirectX::XMUINT4& boneIndices, const DirectX::XMFLOAT4& boneWeights)
		: Position(position), BoneIndices(boneIndices), BoneWeights(boneWeights) { }
} VertexSkinnedPosition;