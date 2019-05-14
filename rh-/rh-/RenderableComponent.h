#pragma once

#include "Component.h"
#include "DeviceResources.h"
#include "Camera.h"
#include "ModelSkinned.h"


class RenderableComponent : public Component
{
public:
	//Methods
	RenderableComponent();
	RenderableComponent(std::wstring path, Camera& camera);
	virtual ~RenderableComponent();

	virtual ComponentType GetType(void) override;

	//Fields
	std::unique_ptr<DirectX::Model>	_model;
	std::unique_ptr<ModelSkinned> _modelSkinned;
	std::wstring _modelPath;
	Camera* _camera;

};
