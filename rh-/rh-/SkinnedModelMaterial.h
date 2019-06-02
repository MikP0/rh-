#pragma once
#include "pch.h"

#include "MaterialSK.h"
#include "VertexDeclarations.h"

class SkinnedModelMaterial : public MaterialSK
{
	RTTI_DECLARATIONS(SkinnedModelMaterial, MaterialSK)

		MATERIAL_VARIABLE_DECLARATION(WorldViewProjection)
		MATERIAL_VARIABLE_DECLARATION(World)
		MATERIAL_VARIABLE_DECLARATION(SpecularColor)
		MATERIAL_VARIABLE_DECLARATION(SpecularPower)
		MATERIAL_VARIABLE_DECLARATION(AmbientColor)
		MATERIAL_VARIABLE_DECLARATION(LightColor)
		MATERIAL_VARIABLE_DECLARATION(LightPosition)
		MATERIAL_VARIABLE_DECLARATION(LightRadius)
		MATERIAL_VARIABLE_DECLARATION(CameraPosition)
		MATERIAL_VARIABLE_DECLARATION(BoneTransforms)
		MATERIAL_VARIABLE_DECLARATION(ColorTexture)
		MATERIAL_VARIABLE_DECLARATION(View)
		MATERIAL_VARIABLE_DECLARATION(Projection)
		MATERIAL_VARIABLE_DECLARATION(Hitted)

public:
	SkinnedModelMaterial();

	void Initialize(EffectSK& effect);
	virtual void CreateVertexBuffer(ID3D11Device* device, const MeshSK& mesh, ID3D11Buffer** vertexBuffer) const override;
	void CreateVertexBuffer(ID3D11Device* device, VertexSkinnedPositionTextureNormal* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
	virtual UINT VertexSize() const override;
};