#include "pch.h"
#include "RenderableSystem.h"
#include "RenderableComponent.h"


RenderableSystem::RenderableSystem(std::shared_ptr<EntityManager> entityManager, ID3D11Device1* device, ID3D11DeviceContext1* context) : System(entityManager)
{
	_device = device;
	_context = context;

	_states = std::make_unique<DirectX::CommonStates>(_device);

	_fxFactory = std::make_unique<DirectX::EffectFactory>(_device);

	_componentsType = ComponentType("Renderable");
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
	//std::shared_ptr<RenderableComponent> temporaryComponent;
	for (auto renderableComponent : _entityManager->GetComponents(_componentsType))
	{
		//temporaryComponent = std::dynamic_pointer_cast<RenderableComponent>(renderableComponent);
		std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model->Draw(
			_context, *_states, renderableComponent->GetParent()->GetWorldMatrix(),
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetViewMatrix(), 
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_camera->GetProjectionMatrix()
		);
	}
}

void RenderableSystem::Initialize()
{
	for (auto renderableComponent : _entityManager->GetComponents(_componentsType)) 
	{
		std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_model = 
			DirectX::Model::CreateFromCMO(_device,
			std::dynamic_pointer_cast<RenderableComponent>(renderableComponent)->_modelPath.c_str(),
			*_fxFactory);
	}
}
