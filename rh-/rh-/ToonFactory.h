#pragma once

#include "pch.h"
#include <DirectXMath.h>
#include <memory>

//------------------------------------------------------------------------------
class ToonFactory : public DirectX::IEffectFactory
{
public:
	explicit ToonFactory(_In_ ID3D11Device* device);
	ToonFactory(ToonFactory&& moveFrom);
	ToonFactory& operator=(ToonFactory&& moveFrom);

	ToonFactory(ToonFactory const&) = delete;
	ToonFactory& operator=(ToonFactory const&) = delete;

	virtual ~ToonFactory();

	// IEffectFactory methods.
	virtual std::shared_ptr<DirectX::IEffect> __cdecl CreateEffect(_In_ const EffectInfo& info, _In_opt_ ID3D11DeviceContext* deviceContext) override;

	virtual void __cdecl CreateTexture(_In_z_ const wchar_t* name, _In_opt_ ID3D11DeviceContext* deviceContext, _Outptr_ ID3D11ShaderResourceView** textureView) override;

	void __cdecl ReleaseCache();

	void __cdecl SetSharing(bool enabled);

	void __cdecl EnableNormalMapEffect(bool enabled);
	void __cdecl EnableForceSRGB(bool forceSRGB);

	void __cdecl SetDirectory(_In_opt_z_ const wchar_t* path);


	// for Effects
	void __cdecl SetWorld(DirectX::FXMMATRIX value);
	//void __cdecl SetView(DirectX::FXMMATRIX value);
	//void __cdecl SetProjection(DirectX::FXMMATRIX value);
	//void __cdecl SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection);
	//void __cdecl SetCameraPosition(DirectX::XMFLOAT3 value);

private:
	// Private implementation.
	class Impl;

	std::shared_ptr<Impl> pImpl;
};