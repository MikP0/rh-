#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"

#include <codecvt>
#include <locale>

RenderableSystem::RenderableSystem(ID3D11Device1* device, ID3D11DeviceContext1* context)
{
	_device = device;
	_context = context;

	_states = std::make_unique<DirectX::CommonStates>(_device);

	_fxFactory = std::make_shared<ShadowFactory>(_device);

	DebugDrawAction = std::make_unique<DebugDraw>(_device, _context);

	_shadowMap = std::make_unique<ShadowMap>(device, 1024, 1024);

	_renderTargetView = nullptr;
	_depthStencilView = nullptr;

	isSent = false;
}

RenderableSystem::~RenderableSystem()
{
}

void RenderableSystem::Iterate()
{
	_shadowMap->BuildShadowTransform();
	_shadowMap->BindDsvAndSetNullRenderTarget(_context);
	_fxFactory->SetRenderingShadowMap(true);

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		if (renderableComponent->_model != nullptr) {
			renderableComponent->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				_shadowMap->_lightView,
				_shadowMap->_lightProj
			);
		}
		else
		{
			renderableComponent->_modelSkinned->DrawModel(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				_shadowMap->_lightView,
				_shadowMap->_lightProj
			);
		}
	}

	_shadowMap->UnbindTargetAndViewport(_context);

	_context->RSSetState(0);
	XMVECTORF32 myColor = { { { 0.0f, 0.0f, 0.0f, 1.000000000f } } };
	_context->ClearRenderTargetView(_renderTargetView, myColor);
	_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_fxFactory->SetShadowMapEnabled(true);
	_fxFactory->SetShadowMap(_shadowMap->GetDepthMapSRV());
	_fxFactory->SetRenderingShadowMap(false);
	_fxFactory->SetShadowMapTransform(_shadowMap->_lightShadowTransform);
	
	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		if (renderableComponent->_model != nullptr) {
			renderableComponent->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				renderableComponent->_camera->GetViewMatrix(),
				renderableComponent->_camera->GetProjectionMatrix()
			);
		}
		else
		{
			renderableComponent->_modelSkinned->DrawModel(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				renderableComponent->_camera->GetViewMatrix(),
				renderableComponent->_camera->GetProjectionMatrix()
			);
		}
	}
}

void RenderableSystem::Initialize()
{

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		if (renderableComponent->_modelPath.find(L".cmo") != std::wstring::npos) 
		{
			renderableComponent->_model =
				DirectX::Model::CreateFromCMO(_device, renderableComponent->_modelPath.c_str(), *_fxFactory);
		}
		else 
		{
			renderableComponent->_modelSkinned =
				std::make_unique<ModelSkinned>(_device, converter.to_bytes(renderableComponent->_modelPath.c_str()), _context);
		}
	}
}

void RenderableSystem::SentDeviceResources(ID3D11RenderTargetView * renderTargetView, ID3D11DepthStencilView * depthStencilView)
{
	if (!isSent)
	{
		_renderTargetView = renderTargetView;
		_depthStencilView = depthStencilView;
		isSent = true;
	}
}