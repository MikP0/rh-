#pragma once
#include "pch.h"
#include "BufferContainer.h"
#include "MaterialSK.h"

struct aiMesh;

class Material;
class ModelMaterial;
class BoneVertexWeights;

class MeshSK
{
	friend class ModelSK;

public:
	~MeshSK();

	ModelSK& GetModel();
	ModelMaterial* GetMaterial();
	const std::string& Name() const;

	const std::vector<DirectX::XMFLOAT3>& Vertices() const;
	const std::vector<DirectX::XMFLOAT3>& Normals() const;
	const std::vector<DirectX::XMFLOAT3>& Tangents() const;
	const std::vector<DirectX::XMFLOAT3>& BiNormals() const;
	const std::vector<std::vector<DirectX::XMFLOAT3>*>& TextureCoordinates() const;
	const std::vector<std::vector<DirectX::XMFLOAT4>*>& VertexColors() const;
	UINT FaceCount() const;
	const std::vector<UINT>& Indices() const;
	const std::vector<BoneVertexWeights>& BoneWeights() const;

	BufferContainer& VertexBuffer();
	BufferContainer& IndexBuffer();

	bool HasCachedVertexBuffer() const;
	bool HasCachedIndexBuffer() const;

	void CreateIndexBuffer(ID3D11Buffer** indexBuffer);

	void CreateCachedVertexAndIndexBuffers(ID3D11Device & device, const MaterialSK & material)
	{
		mVertexBuffer.ReleaseBuffer();
		mIndexBuffer.ReleaseBuffer();

		ID3D11Buffer* buffer = nullptr;
		material.CreateVertexBuffer(&device, *this, &buffer);
		mVertexBuffer.SetBuffer(buffer);
		mVertexBuffer.SetElementCount(mVertices.size());

		buffer = nullptr;
		CreateIndexBuffer(&buffer);
		mIndexBuffer.SetBuffer(buffer);
		mIndexBuffer.SetElementCount(mIndices.size());
	}


private:
	MeshSK(ModelSK& model, aiMesh& mesh);
	MeshSK(const MeshSK& rhs);
	MeshSK& operator=(const MeshSK& rhs);

	ModelSK& mModel;
	ModelMaterial* mMaterial;
	std::string mName;
	std::vector<DirectX::XMFLOAT3> mVertices;
	std::vector<DirectX::XMFLOAT3> mNormals;
	std::vector<DirectX::XMFLOAT3> mTangents;
	std::vector<DirectX::XMFLOAT3> mBiNormals;
	std::vector<std::vector<DirectX::XMFLOAT3>*> mTextureCoordinates;
	std::vector<std::vector<DirectX::XMFLOAT4>*> mVertexColors;
	UINT mFaceCount;
	std::vector<UINT> mIndices;
	std::vector<BoneVertexWeights> mBoneWeights;

	BufferContainer mVertexBuffer;
	BufferContainer mIndexBuffer;
};