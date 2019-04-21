#pragma once

#include "Component.h"
#include "DeviceResources.h"

class RenderableComponent : public Component
{
public:
	RenderableComponent();
	virtual ~RenderableComponent();

	virtual ComponentType GetType(void) override;

private: 
	std::unique_ptr<DirectX::Model>	_model;
	std::unique_ptr<DX::DeviceResources> _deviceResources;
};

