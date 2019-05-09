#include "pch.h"
#include "RenderableComponent.h"


RenderableComponent::RenderableComponent(std::wstring path , Camera& camera)
{
	_modelPath = path;
	_camera = &camera;
	_model = nullptr;
	_modelSkinned = nullptr;
}

RenderableComponent::~RenderableComponent()
{
}

ComponentType RenderableComponent::GetType(void)
{
	return ComponentType("Renderable");
}
