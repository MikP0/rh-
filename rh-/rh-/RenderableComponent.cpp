#include "pch.h"
#include "RenderableComponent.h"


RenderableComponent::RenderableComponent(std::wstring path , Camera* camera)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = false;
	_ignoreShadows = false;
}

RenderableComponent::RenderableComponent(std::wstring path, Camera* camera, bool Shadow, bool ignoreShadow)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = Shadow;
	_ignoreShadows = ignoreShadow;
}


RenderableComponent::~RenderableComponent()
{
}
