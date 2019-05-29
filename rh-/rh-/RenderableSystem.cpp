#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"

#include <codecvt>
#include <locale>

RenderableSystem::RenderableSystem(std::shared_ptr<EntityManager> entityManager, ID3D11Device1* device, ID3D11DeviceContext1* context) : System(entityManager)
{
	_device = device;
	_context = context;

	_states = std::make_unique<DirectX::CommonStates>(_device);

	_fxFactory = std::make_shared<ShadowFactory>(_device);

	_componentsType = ComponentType("Renderable");

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
	_shadowMap->BuildShadowTransform(_context);
	_shadowMap->BindDsvAndSetNullRenderTarget(_context);
	_fxFactory->SetRenderingShadowMap(true);

	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model != nullptr) {
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				_shadowMap->_lightView,
				_shadowMap->_lightProj
			);
		}
		else
		{
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelSkinned->DrawModel(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				_shadowMap->_lightView,
				_shadowMap->_lightProj
			);
		}
	}

	_shadowMap->UnbindTargetAndViewport(_context);

	_context->RSSetState(0);
	_context->ClearRenderTargetView(_renderTargetView, Colors::Silver);
	_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_fxFactory->SetShadowMapEnabled(true);
	_fxFactory->SetShadowMap(_shadowMap->GetDepthMapSRV());
	_fxFactory->SetRenderingShadowMap(false);
	_fxFactory->SetShadowMapTransform(_shadowMap->_lightShadowTransform);


	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model != nullptr) {
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetViewMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetProjectionMatrix()
			);
		}
		else
		{
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelSkinned->DrawModel(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetViewMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetProjectionMatrix()
			);
		}
	}
}

void RenderableSystem::Iterate(FXMMATRIX view, FXMMATRIX projection)
{
	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model != nullptr) {
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				view,
				projection
			);
		}
		else
		{
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelSkinned->DrawModel(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetViewMatrix(),
				std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetProjectionMatrix()
			);
		}
	}
}


void RenderableSystem::Initialize()
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelPath.find(L".cmo") != std::wstring::npos)
		{
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model =
				DirectX::Model::CreateFromCMO(_device,
					std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelPath.c_str(),
					*_fxFactory);
		}
		else
		{
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelSkinned =
				std::make_unique<ModelSkinned>(_device,
					converter.to_bytes(std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelPath.c_str()),
					_context);

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
