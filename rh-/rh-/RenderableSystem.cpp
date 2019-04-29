#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"


RenderableSystem::RenderableSystem(std::shared_ptr<EntityManager> entityManager) : System(entityManager)
{
	_deviceResources = std::make_unique<DX::DeviceResources>();
	_deviceResources->RegisterDeviceNotify(this);

	_device = _deviceResources->GetD3DDevice();
	_context = _deviceResources->GetD3DDeviceContext();

	_states = std::make_unique<DirectX::CommonStates>(_device);

	_fxFactory = std::make_unique<DirectX::EffectFactory>(_device);
}

RenderableSystem::~RenderableSystem()
{
}

void RenderableSystem::OnDeviceLost()
{
	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model.reset();
	}
}

void RenderableSystem::OnDeviceRestored()
{
	Initialize();
}

void RenderableSystem::Iterate()
{
}

void RenderableSystem::Initialize()
{
	std::shared_ptr<RenderableComponent> temporaryComponent;
	for (auto renderableComponent : _entityManager->GetComponents(_componentsType)) 
	{
		temporaryComponent = std::dynamic_pointer_cast<RenderableComponent>(renderableComponent);
		std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model = 
			DirectX::Model::CreateFromCMO(_device, temporaryComponent->_modelPath.c_str(), *_fxFactory);
	}
}
