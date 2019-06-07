#pragma once

#include "pch.h"

class ToonEffect : public DirectX::IEffect, public DirectX::IEffectMatrices
{
public:
	explicit ToonEffect(_In_ ID3D11Device* device);

	ToonEffect(ToonEffect&& moveFrom);
	ToonEffect& operator= (ToonEffect&& moveFrom);

	ToonEffect(ToonEffect const&) = delete;
	ToonEffect& operator= (ToonEffect const&) = delete;

	~ToonEffect();

	// IEffect methods.
	void __cdecl Apply(_In_ ID3D11DeviceContext* deviceContext) override;
	void __cdecl GetVertexShaderBytecode(_Out_ void const** pShaderByteCode, _Out_ size_t* pByteCodeLength) override;

	// Camera settings.
	void XM_CALLCONV SetWorld(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetView(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetProjection(DirectX::FXMMATRIX value) override;
	void XM_CALLCONV SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;
	void SetCameraPosition(DirectX::XMFLOAT3 value);


	// ToonEffect methods
	void SetTextureEnabled(bool value);
	void SetTexture(_In_opt_ ID3D11ShaderResourceView* value);
	void SetNormalMapEnabled(bool value);
	void SetNormalMap(_In_opt_ ID3D11ShaderResourceView* value);

	// ToonEffect Materials
	void SetDiffuseColor(DirectX::XMFLOAT3 value);
	void SetEmissiveColor(DirectX::XMFLOAT3 value);
	void SetAlpha(float value);

	// Lights
	void AddPointLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Position, float Radius);
	void AddDirectLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction);
	void AddSpotLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction, float OuterAngle, DirectX::XMFLOAT3 Position, float InnerAngle, float Radius);
	void UpdatePointLight(int id, DirectX::XMFLOAT3 Position);
	void UpdateSpotLight(int id, DirectX::XMFLOAT3 Position);

	void SetMaterialName(std::string Name);
	std::string GetMaterialName();

private:
	// Private implementation.
	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};

