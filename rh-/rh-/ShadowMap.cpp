#include "ShadowMap.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

ShadowMap::ShadowMap(_In_ ID3D11Device* device, UINT width, UINT height)
{
	_width = width;
	_height = height;

	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;
	_viewport.Width = static_cast<float>(width);
	_viewport.Height = static_cast<float>(height);
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = _width;
	texDesc.Height = _height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthMap = 0;
	DX::ThrowIfFailed(
		device->CreateTexture2D(&texDesc, 0, depthMap.ReleaseAndGetAddressOf())
	);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed(
		device->CreateDepthStencilView(depthMap.Get(), &dsvDesc, &_depthMapDSV)
	);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DX::ThrowIfFailed(
		device->CreateShaderResourceView(depthMap.Get(), &srvDesc, &_depthMapSRV)
	);

	if (depthMap)
	{
		depthMap.Reset();
	}
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext * context)
{
	UINT numViewports = 1;
	context->RSGetViewports(&numViewports, &orgViewport);
	context->RSSetViewports(1, &_viewport);

	context->OMGetRenderTargets(1, &orgRenderTarger, &orgDepthStencilView);
	
	ID3D11ShaderResourceView* textures[3];
	textures[0] = NULL;
	textures[1] = NULL;
	textures[2] = NULL;
	context->PSSetShaderResources(
		0, static_cast<UINT>(3), textures);

	ID3D11RenderTargetView* pNullRTV = NULL;
	context->OMSetRenderTargets(1, &pNullRTV, NULL);
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	context->OMSetRenderTargets(1, renderTargets, _depthMapDSV);

	context->ClearDepthStencilView(_depthMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void ShadowMap::UnbindTargetAndViewport(ID3D11DeviceContext * context)
{
	context->RSSetViewports(1, &orgViewport);

	ID3D11ShaderResourceView* textures[3];
	textures[0] = NULL;
	textures[1] = NULL;
	textures[2] = NULL;
	context->PSSetShaderResources(
		0, static_cast<UINT>(3), textures);

	ID3D11RenderTargetView* pNullRTV = NULL;
	context->OMSetRenderTargets(1, &pNullRTV, NULL);

	ID3D11RenderTargetView* renderTargets[1] = { orgRenderTarger };
	context->OMSetRenderTargets(1, renderTargets, orgDepthStencilView);
}

ShadowMap::~ShadowMap()
{
}

ShadowMap::ShadowMap(ShadowMap&& /*moveFrom*/)
{
}

ID3D11ShaderResourceView * ShadowMap::GetDepthMapSRV()
{
	return _depthMapSRV;
}

void ShadowMap::BuildShadowTransform()
{
	DirectX::XMVECTOR lightDir = DirectX::XMVectorSet(2.0f, -2.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR eye = -lightDir * 10;
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX smView = DirectX::XMMatrixLookAtLH(eye, at, up);
	DirectX::XMMATRIX smProjection = DirectX::XMMatrixOrthographicLH(20.f, 20.f, 0.1f, 100.f);

	/*Matrix T;
	T._11 = 0;
	T._12 = 0;
	T._13 = 0;
	T._14 = 0;
	T._21 = 0;
	T._22 = 0;
	T._23 = 0;
	T._24 = 0;
	T._31 = 0;
	T._32 = 0;
	T._33 = 0;
	T._34 = 0;
	T._41 = 0;
	T._42 = 0;
	T._43 = 0;
	T._44 = 0;

	T._11 = 0.5f;
	T._22 = -0.5f;
	T._33 = 1.0f;
	T._41 = 0.5f;
	T._42 = 0.5f;
	T._44 = 1.0f;

	XMMATRIX TT = T;*/

	XMMATRIX res = smView * smProjection;

	_lightShadowTransform = res;
	_lightProj = smProjection;
	_lightView = smView;
}