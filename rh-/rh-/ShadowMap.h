#pragma once

#include "pch.h"

class ShadowMap
{
public:
	ShadowMap(_In_ ID3D11Device* device, UINT width, UINT height);
	ShadowMap(ShadowMap&& moveFrom);
	ShadowMap& operator= (ShadowMap&& moveFrom);
	 ~ShadowMap();

	ID3D11ShaderResourceView * GetDepthMapSRV();
	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext * context);
	void UnbindTargetAndViewport(ID3D11DeviceContext * context);

	void BuildShadowTransform(DirectX::SimpleMath::Vector3 pos);

	DirectX::XMMATRIX _lightView;
	DirectX::XMMATRIX _lightProj;
	DirectX::XMMATRIX _lightShadowTransform;

private:

	ShadowMap(ShadowMap const&);
	ShadowMap& operator= (ShadowMap const&);

	UINT _width;
	int _height;
	ID3D11ShaderResourceView* _depthMapSRV;
	ID3D11DepthStencilView* _depthMapDSV;
	D3D11_VIEWPORT _viewport;

	D3D11_VIEWPORT orgViewport;
	ID3D11RenderTargetView* orgRenderTarger;
	ID3D11DepthStencilView * orgDepthStencilView;
};

