#include "pch.h"
#include "ShadowModelMaterial.h"
#include "GameException.h"
#include "MeshSK.h"
#include "Bone.h"

RTTI_DEFINITIONS(ShadowModelMaterial)

ShadowModelMaterial::ShadowModelMaterial()
	: MaterialSK("mainShadow"),
	MATERIAL_VARIABLE_INITIALIZATION(WorldViewProjection), MATERIAL_VARIABLE_INITIALIZATION(BoneTransforms)
{
}

MATERIAL_VARIABLE_DEFINITION(ShadowModelMaterial, WorldViewProjection)
MATERIAL_VARIABLE_DEFINITION(ShadowModelMaterial, BoneTransforms)

void ShadowModelMaterial::Initialize(EffectSK& effect)
{
	MaterialSK::Initialize(effect);

	MATERIAL_VARIABLE_RETRIEVE(WorldViewProjection)	
	MATERIAL_VARIABLE_RETRIEVE(BoneTransforms)

		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	CreateInputLayout("mainShadow", "p0", inputElementDescriptions, ARRAYSIZE(inputElementDescriptions));
}

void ShadowModelMaterial::CreateVertexBuffer(ID3D11Device* device, const MeshSK& mesh, ID3D11Buffer** vertexBuffer) const
{
	const std::vector<DirectX::XMFLOAT3>& sourceVertices = mesh.Vertices();
	const std::vector<BoneVertexWeights>& boneWeights = mesh.BoneWeights();
	assert(boneWeights.size() == sourceVertices.size());

	std::vector<VertexSkinnedPosition> vertices;
	vertices.reserve(sourceVertices.size());
	for (UINT i = 0; i < sourceVertices.size(); i++)
	{
		DirectX::XMFLOAT3 position = sourceVertices.at(i);
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

		vertices.push_back(VertexSkinnedPosition((DirectX::XMFLOAT4(position.x, position.y, position.z, 1.0f)), DirectX::XMUINT4(indices), DirectX::XMFLOAT4(weights)));
	}

	CreateVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);
}

void ShadowModelMaterial::CreateVertexBuffer(ID3D11Device* device, VertexSkinnedPosition* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const
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

UINT ShadowModelMaterial::VertexSize() const
{
	return sizeof(VertexSkinnedPosition);
}