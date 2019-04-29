#pragma once

#include "System.h"
#include "DeviceResources.h"
#include "StepTimer.h"

class RenderableSystem :
	public System, DX::IDeviceNotify
{
public:
	
	RenderableSystem(std::shared_ptr<EntityManager> entityManager);
	~RenderableSystem();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;


	virtual void Iterate() override;
	virtual void Initialize() override;

	//Fields
	std::unique_ptr<DX::DeviceResources> _deviceResources;
	std::unique_ptr<DirectX::CommonStates> _states;
	std::unique_ptr<DirectX::IEffectFactory> _fxFactory;

	ID3D11Device1* _device;
	ID3D11DeviceContext1* _context;
};

