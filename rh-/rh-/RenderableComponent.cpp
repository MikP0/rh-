#include "pch.h"
#include "RenderableComponent.h"


//RenderableComponent::RenderableComponent()
//{
//	_device = _deviceResources->GetD3DDevice();
//	_context = _deviceResources->GetD3DDeviceContext();
//
//	_states = std::make_unique<DirectX::CommonStates>(_device);
//
//	_fxFactory = std::make_unique<DirectX::EffectFactory>(_device);
//	_modelPath = "";
//}

RenderableComponent::RenderableComponent(std::string path)
{
}

RenderableComponent::~RenderableComponent()
{
}

ComponentType RenderableComponent::GetType(void)
{
	return ComponentType("Renderable");
}
