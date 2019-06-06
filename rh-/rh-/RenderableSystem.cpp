#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"

#include <codecvt>
#include <locale>

RenderableSystem::RenderableSystem(ID3D11Device1* device, ID3D11DeviceContext1* context, shared_ptr<PhysicsSystem> physicsSystem)
{
	_device = device;
	_context = context;

	_states = std::make_unique<DirectX::CommonStates>(_device);

	_ShadowsfxFactory = std::make_shared<ShadowFactory>(_device);
	_noShadowsfxFactory = std::make_shared<ToonFactory>(_device);

	DebugDrawAction = std::make_unique<DebugDraw>(_device, _context);

	_shadowMap = std::make_unique<ShadowMap>(device, 1024, 1024);

	_renderTargetView = nullptr;
	_depthStencilView = nullptr;

	_isSent = false;

	_player = nullptr;

	_physicsSystem = physicsSystem;
}

RenderableSystem::~RenderableSystem()
{
}

void RenderableSystem::Iterate()
{
	PrepareToRenderShadows();

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		//vector<int> objectsToRender = _physicsSystem->GetEntitiesIDWithinFrustum();
		//std::vector<int>::iterator it = std::find(objectsToRender.begin(), objectsToRender.end(), renderableComponent->GetParent()->GetId());

		/*if (objectsToRender.size() == 0)
			OutputDebugStringW(L"Zero\n");
		else
			if (objectsToRender.size() == 1)
				OutputDebugStringW(L"Jeden\n");
			else
				if (objectsToRender.size() == 2)
					OutputDebugStringW(L"Dwa\n");*/

					//if (it != objectsToRender.end())
					//{

		if (renderableComponent->_model == nullptr)
		{
			if (renderableComponent->_modelSkinned->isVisible)
			{
				renderableComponent->_modelSkinned->drawToShadows = true;

				if (renderableComponent->_modelSkinned->playingAnimation)
				{
					renderableComponent->_modelSkinned->GetAnimatorPlayer()->StartClip(renderableComponent->_modelSkinned->currentAnimation);
					renderableComponent->_modelSkinned->GetAnimatorPlayer()->Update(Coroutine::GetElapsedTime());	// update animation
				}

				renderableComponent->_modelSkinned->DrawModel(
					_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
					_shadowMap->_lightView,
					_shadowMap->_lightProj
				);
				//}
				renderableComponent->_modelSkinned->drawToShadows = false;
			}
		}
		//}
	}

	ClearAfterRenderShadows();

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		vector<int> objectsToRender = _physicsSystem->GetEntitiesIDWithinFrustum();
		std::vector<int>::iterator it = std::find(objectsToRender.begin(), objectsToRender.end(), renderableComponent->GetParent()->GetId());

		//if (it != objectsToRender.end())
		//{
		if (renderableComponent->_model != nullptr) {
			renderableComponent->_model->Draw(
				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
				renderableComponent->_camera->GetViewMatrix(),
				renderableComponent->_camera->GetProjectionMatrix()
			);
		}
		else
		{
			if (renderableComponent->_modelSkinned->isVisible)
			{
				renderableComponent->_modelSkinned->DrawModel(
					_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
					renderableComponent->_camera->GetViewMatrix(),
					renderableComponent->_camera->GetProjectionMatrix()
				);
			}
		}
		//}
	}
}

void RenderableSystem::Initialize()
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		if (renderableComponent->_modelPath.find(L".cmo") != std::wstring::npos)
		{
			if (renderableComponent->_canRenderShadows)
			{
				renderableComponent->_model =
					DirectX::Model::CreateFromCMO(_device, renderableComponent->_modelPath.c_str(), *_ShadowsfxFactory);
			}
			else
			{
				renderableComponent->_model =
					DirectX::Model::CreateFromCMO(_device, renderableComponent->_modelPath.c_str(), *_noShadowsfxFactory);
			}
		}
		else
		{
			renderableComponent->_modelSkinned =
				std::make_unique<ModelSkinned>(_device, converter.to_bytes(renderableComponent->_modelPath.c_str()), _context);
		}
	}
}

void RenderableSystem::SentResources(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, std::shared_ptr<Entity> Player)
{
	if (!_isSent)
	{
		_renderTargetView = renderTargetView;
		_depthStencilView = depthStencilView;
		_player = Player;
		_isSent = true;
	}
}

void RenderableSystem::PrepareToRenderShadows()
{
	_shadowMap->BuildShadowTransform(_player->GetTransform()->GetPosition());
	_shadowMap->BindDsvAndSetNullRenderTarget(_context);
	//_ShadowsfxFactory->SetRenderingShadowMap(true);

	_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderableSystem::ClearAfterRenderShadows()
{
	_shadowMap->UnbindTargetAndViewport(_context);

	_context->RSSetState(0);
	XMVECTORF32 myColor = { { { 0.0f, 0.0f, 0.0f, 1.000000000f } } };
	_context->ClearRenderTargetView(_renderTargetView, Colors::Silver);
	_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_ShadowsfxFactory->SetShadowMapEnabled(true);
	_ShadowsfxFactory->SetShadowMap(_shadowMap->GetDepthMapSRV());
	//_ShadowsfxFactory->SetRenderingShadowMap(false);
	_ShadowsfxFactory->SetShadowMapTransform(_shadowMap->_lightShadowTransform);
}
