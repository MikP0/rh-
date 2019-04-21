#include "pch.h"
#include "RenderableComponent.h"


RenderableComponent::RenderableComponent()
{
	auto device = _deviceResources->GetD3DDevice();
	auto context = _deviceResources->GetD3DDeviceContext();

	_states = std::make_unique<DirectX::CommonStates>(device);

	_fxFactory = std::make_unique<DirectX::EffectFactory>(device);
	_modelPath = "";
}

RenderableComponent::RenderableComponent(std::string path)
{
	auto device = _deviceResources->GetD3DDevice();
	auto context = _deviceResources->GetD3DDeviceContext();

	_states = std::make_unique<DirectX::CommonStates>(device);

	_fxFactory = std::make_unique<DirectX::EffectFactory>(device);

	_modelPath = path;
}

RenderableComponent::~RenderableComponent()
{
}

ComponentType RenderableComponent::GetType(void)
{
	return ComponentType("Renderable");
}
