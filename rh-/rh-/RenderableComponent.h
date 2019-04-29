#pragma once

#include "Component.h"
#include "DeviceResources.h"

class RenderableComponent : public Component
{
public:
	//Methods
	RenderableComponent();
	RenderableComponent(std::string path);
	virtual ~RenderableComponent();

	virtual ComponentType GetType(void) override;

	//Fields
	std::unique_ptr<DirectX::Model>	_model;
	std::wstring _modelPath;

};

