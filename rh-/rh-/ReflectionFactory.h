#pragma once
#include "pch.h"
#include <DirectXMath.h>
#include <memory>


class ReflectionFactory : public DirectX::IEffectFactory
{
public:
	explicit ReflectionFactory(_In_ ID3D11Device* device);
	ReflectionFactory(ReflectionFactory&& moveFrom);
	ReflectionFactory& operator=(ReflectionFactory&& moveFrom);

	ReflectionFactory(ReflectionFactory const&) = delete;
	ReflectionFactory& operator=(ReflectionFactory const&) = delete;

	virtual ~ReflectionFactory();

	// IEffectFactory methods.
	virtual std::shared_ptr<DirectX::IEffect> __cdecl CreateEffect(_In_ const EffectInfo& info, _In_opt_ ID3D11DeviceContext* deviceContext) override;

	virtual void __cdecl CreateTexture(_In_z_ const wchar_t* name, _In_opt_ ID3D11DeviceContext* deviceContext, _Outptr_ ID3D11ShaderResourceView** textureView) override;

	void __cdecl ReleaseCache();

	void __cdecl SetSharing(bool enabled);

	void __cdecl EnableNormalMapEffect(bool enabled);
	void __cdecl EnableForceSRGB(bool forceSRGB);

	void __cdecl SetDirectory(_In_opt_z_ const wchar_t* path);


	void SetCameraPosition(DirectX::XMFLOAT3 value);

	void SetCubeMap(ID3D11ShaderResourceView* value);


	// Lights
	void AddPointLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Position, float Radius);
	void AddDirectLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction);
	void AddSpotLight(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction, float OuterAngle, DirectX::XMFLOAT3 Position, float InnerAngle, float Radius);
	void UpdatePointLight(int id, DirectX::XMFLOAT3 Position);
	void UpdateSpotLight(int id, DirectX::XMFLOAT3 Position);

private:
	// Private implementation.
	class Impl;

	std::shared_ptr<Impl> pImpl;
};