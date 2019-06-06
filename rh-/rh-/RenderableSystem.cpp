#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"
#include "PostProcess.h"

#include <codecvt>
#include <locale>

RenderableSystem::RenderableSystem(ID3D11Device1* device, ID3D11DeviceContext1* context, shared_ptr<PhysicsSystem> physicsSystem)
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

	_physicsSystem = physicsSystem;

	_screenSizeChanged = true;
	_postProcess = std::make_unique<BasicPostProcess>(_device);
}

RenderableSystem::~RenderableSystem()
{
}

void RenderableSystem::CreateScreenTextureResources()
{
	CD3D11_TEXTURE2D_DESC sceneDesc(
		DXGI_FORMAT_R16G16B16A16_FLOAT, _screenWidth, _screenHeight,
		1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	DX::ThrowIfFailed(
		_device->CreateTexture2D(&sceneDesc, nullptr, _sceneTex.GetAddressOf())
	);

	DX::ThrowIfFailed(
		_device->CreateShaderResourceView(_sceneTex.Get(), nullptr,
			_sceneSRV.ReleaseAndGetAddressOf())
	);

	DX::ThrowIfFailed(
		_device->CreateRenderTargetView(_sceneTex.Get(), nullptr,
			_sceneRT.ReleaseAndGetAddressOf()
		));
}

void RenderableSystem::Iterate()
{
	if (_screenSizeChanged)
	{
		CreateScreenTextureResources();
		_screenSizeChanged = false;
	}

	//_shadowMap->BuildShadowTransform();
	//_shadowMap->BindDsvAndSetNullRenderTarget(_context);
	//_fxFactory->SetRenderingShadowMap(true);

	vector<int> objectsToRender = _physicsSystem->GetEntitiesIDWithinFrustum();

	//for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	//{
	//	std::vector<int>::iterator it = std::find(objectsToRender.begin(), objectsToRender.end(), renderableComponent->GetParent()->GetId());

	//	/*if (objectsToRender.size() == 0)
	//		OutputDebugStringW(L"Zero\n");
	//	else
	//		if (objectsToRender.size() == 1)
	//			OutputDebugStringW(L"Jeden\n");
	//		else
	//			if (objectsToRender.size() == 2)
	//				OutputDebugStringW(L"Dwa\n");*/

	//	//if (it != objectsToRender.end())
	//	//{
	//		if (renderableComponent->_model != nullptr) {
	//			renderableComponent->_model->Draw(
	//				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
	//				_shadowMap->_lightView,
	//				_shadowMap->_lightProj
	//			);
	//		}
	//		else
	//		{
	//			if (renderableComponent->_modelSkinned->playingAnimation)
	//			{
	//				renderableComponent->_modelSkinned->GetAnimatorPlayer()->StartClip(renderableComponent->_modelSkinned->currentAnimation);
	//				renderableComponent->_modelSkinned->GetAnimatorPlayer()->Update(Coroutine::GetElapsedTime());	// update animation
	//			}

	//			renderableComponent->_modelSkinned->DrawModel(
	//				_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
	//				_shadowMap->_lightView,
	//				_shadowMap->_lightProj
	//			);
	//		}
	//	//}
	//}

	//_shadowMap->UnbindTargetAndViewport(_context);

	//_context->RSSetState(0);
	XMVECTORF32 myColor = { { { 0.0f, 0.0f, 0.0f, 1.000000000f } } };
	//_context->ClearRenderTargetView(_renderTargetView, myColor);
	//_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_context->ClearRenderTargetView(_sceneRT.Get(), myColor);
	_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_context->OMSetRenderTargets(1, _sceneRT.GetAddressOf(), _depthStencilView);

	//_fxFactory->SetShadowMapEnabled(true);
	//_fxFactory->SetShadowMap(_shadowMap->GetDepthMapSRV());
	//_fxFactory->SetRenderingShadowMap(false);
	//_fxFactory->SetShadowMapTransform(_shadowMap->_lightShadowTransform);
	//

	shared_ptr<RenderableComponent> player;
	vector<shared_ptr<RenderableComponent>> enemies;

	for (auto renderableComponent : _world->GetComponents<RenderableComponent>())
	{
		if (renderableComponent->GetParent()->GetTag() == PLAYER)
		{
			player = renderableComponent;
			continue;
		}

		std::vector<int>::iterator it = std::find(objectsToRender.begin(), objectsToRender.end(), renderableComponent->GetParent()->GetId());

		//if (it != objectsToRender.end())
		//{
		if (renderableComponent->GetParent()->GetTag() == ENEMY)
		{
			enemies.push_back(renderableComponent);
			continue;
		}

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

				if (renderableComponent->_modelSkinned->playingAnimation)
				{
					renderableComponent->_modelSkinned->GetAnimatorPlayer()->StartClip(renderableComponent->_modelSkinned->currentAnimation);
					renderableComponent->_modelSkinned->GetAnimatorPlayer()->Update(Coroutine::GetElapsedTime());	// update animation

				}
				renderableComponent->_modelSkinned->DrawModel(
					_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
					renderableComponent->_camera->GetViewMatrix(),
					renderableComponent->_camera->GetProjectionMatrix()
				);
			}
		}
		//}
	}

	BloomBlur();

	// player render
	//if (it != objectsToRender.end())
	//{
	if (player->_modelSkinned->isVisible)
	{
		if (player->_modelSkinned->playingAnimation)
		{
			player->_modelSkinned->GetAnimatorPlayer()->StartClip(player->_modelSkinned->currentAnimation);
			player->_modelSkinned->GetAnimatorPlayer()->Update(Coroutine::GetElapsedTime());	// update animation

		}
		player->_modelSkinned->DrawModel(
			_context, *_states, player->GetParent()->GetWorldMatrix(),
			player->_camera->GetViewMatrix(),
			player->_camera->GetProjectionMatrix()
		);
	}
	//}


	// enemies render
	for (auto enemy : enemies)
	{
		if (enemy->_modelSkinned->playingAnimation)
		{
			enemy->_modelSkinned->GetAnimatorPlayer()->StartClip(enemy->_modelSkinned->currentAnimation);
			enemy->_modelSkinned->GetAnimatorPlayer()->Update(Coroutine::GetElapsedTime());	// update animation

		}
		enemy->_modelSkinned->DrawModel(
			_context, *_states, enemy->GetParent()->GetWorldMatrix(),
			enemy->_camera->GetViewMatrix(),
			enemy->_camera->GetProjectionMatrix()
		);
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

void RenderableSystem::SentDeviceResources(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, int screenWidth, int screenHeight)
{
	if (!isSent)
	{
		_renderTargetView = renderTargetView;
		_depthStencilView = depthStencilView;
		isSent = true;
	}

	if (_screenWidth != screenWidth || _screenHeight != screenHeight)
	{
		_screenSizeChanged = true;
		_screenWidth = screenWidth;
		_screenHeight = screenHeight;
	}
}

void RenderableSystem::BloomBlur()
{
	_context->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
	_postProcess->SetEffect(BasicPostProcess::BloomBlur);
	_postProcess->SetBloomBlurParameters(BloomBlurParams.horizontal, BloomBlurParams.size, BloomBlurParams.brightness);
	_postProcess->SetSourceTexture(_sceneSRV.Get());
	_postProcess->Process(_context);
}
