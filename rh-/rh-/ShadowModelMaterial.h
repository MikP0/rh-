#pragma once
#include "pch.h"

#include "MaterialSK.h"
#include "VertexDeclarations.h"

class ShadowModelMaterial : public MaterialSK
{
	RTTI_DECLARATIONS(ShadowModelMaterial, MaterialSK)

	MATERIAL_VARIABLE_DECLARATION(WorldViewProjection)
	MATERIAL_VARIABLE_DECLARATION(BoneTransforms)

public:
	ShadowModelMaterial();

	void Initialize(EffectSK& effect);
	virtual void CreateVertexBuffer(ID3D11Device* device, const MeshSK& mesh, ID3D11Buffer** vertexBuffer) const override;
	void CreateVertexBuffer(ID3D11Device* device, VertexSkinnedPosition* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
	virtual UINT VertexSize() const override;
};