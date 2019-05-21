#pragma once

#include "pch.h"

#include "MaterialSK.h"
#include "VertexDeclarations.h"


//class DepthMapMaterial : public MaterialSK
//{
//	RTTI_DECLARATIONS(DepthMapMaterial, MaterialSK)
//
//	MATERIAL_VARIABLE_DECLARATION(WorldLightViewProjection)
//
//public:
//	DepthMapMaterial();
//
//	virtual void Initialize(EffectSK& effect) override;
//	virtual void CreateVertexBuffer(ID3D11Device* device, const MeshSK& mesh, ID3D11Buffer** vertexBuffer) const override;
//	void CreateVertexBuffer(ID3D11Device* device, VertexPosition* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
//	virtual UINT VertexSize() const override;
//};