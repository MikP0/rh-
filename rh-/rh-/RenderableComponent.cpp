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
	_canReflect = false;
	_ignoreShadows = false;
}

RenderableComponent::RenderableComponent(std::wstring path, Camera* camera, bool Shadow, bool ignoreShadow)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = Shadow;
	_canCastShadows = false;
	_canReflect = false;
	_ignoreShadows = ignoreShadow;
}

RenderableComponent::RenderableComponent(std::wstring path, Camera* camera, bool Shadow, bool CastShadow)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = Shadow;
	_canCastShadows = CastShadow;
	_canReflect = false;
	_ignoreShadows = false;
}

RenderableComponent::RenderableComponent(std::wstring path, Camera * camera, bool Shadow, bool CastShadow, bool Reflection)
{
	_modelPath = path;
	_camera = camera;
	_model = nullptr;
	_modelSkinned = nullptr;
	_canRenderShadows = Shadow;
	_canCastShadows = CastShadow;
	_canReflect = Reflection;
	_ignoreShadows = false;
}

RenderableComponent::~RenderableComponent()
{
}
