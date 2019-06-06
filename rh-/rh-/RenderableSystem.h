#pragma once

#include "System.h"
#include "DeviceResources.h"
#include "StepTimer.h"
#include "ToonFactory.h"
#include "DebugDraw.h"
#include "PhysicsSystem.h"

#include "ShadowMap.h"
#include "ShadowFactory.h"
#include "Coroutine.h"

class RenderableSystem :
	public System
{
public:
	
	RenderableSystem(ID3D11Device1* device, ID3D11DeviceContext1* context, shared_ptr<PhysicsSystem> physicsSystem);
	~RenderableSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;

	//Fields
	std::unique_ptr<DX::DeviceResources> _deviceResources;
	std::unique_ptr<DirectX::CommonStates> _states;
	std::shared_ptr<ShadowFactory> _fxFactory;

	ID3D11Device1* _device;
	ID3D11DeviceContext1* _context;

	bool Debug;
	std::unique_ptr<DebugDraw> DebugDrawAction;

	std::unique_ptr<ShadowMap> _shadowMap;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11DepthStencilView* _depthStencilView;
	bool _isSent;

	std::shared_ptr<Entity> _player;

	void SentResources(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, std::shared_ptr<Entity> Player);

private:
	shared_ptr<PhysicsSystem> _physicsSystem;
};

