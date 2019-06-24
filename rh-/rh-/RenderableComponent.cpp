#include "pch.h"
#include "RenderableComponent.h"


RenderableComponent::RenderableComponent(std::wstring path , Camera* camera)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = false;
	_canCastShadows = false;
}

RenderableComponent::RenderableComponent(std::wstring path, Camera* camera, bool Shadow)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = Shadow;
	_canCastShadows = false;
}

RenderableComponent::RenderableComponent(std::wstring path, Camera* camera, bool Shadow, bool CastShadow)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = Shadow;
	_canCastShadows = CastShadow;
}

RenderableComponent::~RenderableComponent()
{
}
