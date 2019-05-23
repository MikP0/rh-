#pragma once

#include "System.h"
#include "DeviceResources.h"
#include "StepTimer.h"
#include "ToonFactory.h"


class RenderableSystem :
	public System
{
public:
	
	RenderableSystem(ID3D11Device1* device, ID3D11DeviceContext1* context);
	~RenderableSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;

	//Fields
	std::unique_ptr<DX::DeviceResources> _deviceResources;
	std::shared_ptr<DirectX::CommonStates> _states;
	std::shared_ptr<DirectX::IEffectFactory> _fxFactory;

	ID3D11Device1* _device;
	ID3D11DeviceContext1* _context;
};

