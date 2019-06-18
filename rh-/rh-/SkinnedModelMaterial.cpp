#include "pch.h"
#include "SkinnedModelMaterial.h"
#include "GameException.h"
#include "MeshSK.h"
#include "Bone.h"


RTTI_DEFINITIONS(SkinnedModelMaterial)

SkinnedModelMaterial::SkinnedModelMaterial()
	: MaterialSK("mainRender"),
	MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection), MATERIAL_VARIABLE_INITIALIZATION(World),
	MATERIAL_VARIABLE_INITIALIZATION(SpecularColor), MATERIAL_VARIABLE_INITIALIZATION(SpecularPower),
	MATERIAL_VARIABLE_INITIALIZATION(AmbientColor), MATERIAL_VARIABLE_INITIALIZATION(LightColor),
	MATERIAL_VARIABLE_INITIALIZATION(LightPosition), MATERIAL_VARIABLE_INITIALIZATION(LightRadius),
	MATERIAL_VARIABLE_INITIALIZATION(CameraPosition), MATERIAL_VARIABLE_INITIALIZATION(BoneTransforms),
	MATERIAL_VARIABLE_INITIALIZATION(ColorTexture), MATERIAL_VARIABLE_INITIALIZATION(View),
	MATERIAL_VARIABLE_INITIALIZATION(Projection), MATERIAL_VARIABLE_INITIALIZATION(Hitted)
{
}

MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, WorldViewProjection)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, World)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, SpecularColor)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, SpecularPower)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, AmbientColor)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, LightColor)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, LightPosition)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, LightRadius)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, CameraPosition)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, BoneTransforms)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, ColorTexture)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, View)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, Projection)
MATERIAL_VARIABLE_DEFINITION(SkinnedModelMaterial, Hitted)

void SkinnedModelMaterial::Initialize(EffectSK& effect)
{
	MaterialSK::Initialize(effect);

	MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection)
		MATERIAL_VARIABLE_RETRIEVE(World)
		MATERIAL_VARIABLE_RETRIEVE(SpecularColor)
		MATERIAL_VARIABLE_RETRIEVE(SpecularPower)
		MATERIAL_VARIABLE_RETRIEVE(AmbientColor)
		MATERIAL_VARIABLE_RETRIEVE(LightColor)
		MATERIAL_VARIABLE_RETRIEVE(LightPosition)
		MATERIAL_VARIABLE_RETRIEVE(LightRadius)
		MATERIAL_VARIABLE_RETRIEVE(CameraPosition)
		MATERIAL_VARIABLE_RETRIEVE(BoneTransforms)
		MATERIAL_VARIABLE_RETRIEVE(ColorTexture)
		MATERIAL_VARIABLE_RETRIEVE(View)
		MATERIAL_VARIABLE_RETRIEVE(Projection)
		MATERIAL_VARIABLE_RETRIEVE(Hitted)

		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	CreateInputLayout("mainRender", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));
}

void SkinnedModelMaterial::CreateVertexBuffer(ID3D11Device* device, const MeshSK& mesh, ID3D11Buffer** vertexBuffer) const
{
	const std::vector<DirectX::XMFLOAT3>& sourceVertices = mesh.Vertices();
	std::vector<DirectX::XMFLOAT3>* textureCoordinates = mesh.TextureCoordinates().at(0);
	assert(textureCoordinates->size() == sourceVertices.size());
	const std::vector<DirectX::XMFLOAT3>& normals = mesh.Normals();
	assert(normals.size() == sourceVertices.size());
	const std::vector<BoneVertexWeights>& boneWeights = mesh.BoneWeights();
	assert(boneWeights.size() == sourceVertices.size());

	std::vector<VertexSkinnedPositionTextureNormal> vertices;
	vertices.reserve(sourceVertices.size());
	for (UINT i = 0; i < sourceVertices.size(); i++)
	{
		DirectX::XMFLOAT3 position = sourceVertices.at(i);
		DirectX::XMFLOAT3 uv = textureCoordinates->at(i);
		DirectX::XMFLOAT3 normal = normals.at(i);
		BoneVertexWeights vertexWeights = boneWeights.at(i);

		float weights[BoneVertexWeights::MaxBoneWeightsPerVertex];
		UINT indices[BoneVertexWeights::MaxBoneWeightsPerVertex];
		ZeroMemory(weights, sizeof(float) * ARRAYSIZE(weights));
		ZeroMemory(indices, sizeof(UINT) * ARRAYSIZE(indices));
		for (UINT i = 0; i < vertexWeights.Weights().size(); i++)
		{
			BoneVertexWeights::VertexWeight vertexWeight = vertexWeights.Weights().at(i);
			weights[i] = vertexWeight.Weight;
			indices[i] = vertexWeight.BoneIndex;
		}

		vertices.push_back(VertexSkinnedPositionTextureNormal(DirectX::XMFLOAT4(position.x, position.y, position.z, 1.0f), DirectX::XMFLOAT2(uv.x, uv.y), normal, DirectX::XMUINT4(indices), DirectX::XMFLOAT4(weights)));
	}

	CreateVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);
}

void SkinnedModelMaterial::CreateVertexBuffer(ID3D11Device* device, VertexSkinnedPositionTextureNormal* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.ByteWidth = VertexSize() * vertexCount;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubResourceData;
	ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
	vertexSubResourceData.pSysMem = vertices;
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer)))
	{
		throw GameException("ID3D11Device::CreateBuffer() failed.");
	}
}

UINT SkinnedModelMaterial::VertexSize() const
{
	return sizeof(VertexSkinnedPositionTextureNormal);
}