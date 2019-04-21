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
	std::unique_ptr<DX::DeviceResources> _deviceResources;
	std::unique_ptr<DirectX::CommonStates> _states;
	std::unique_ptr<DirectX::IEffectFactory> _fxFactory;

	std::unique_ptr<DirectX::Model>	_model;
	std::string _modelPath;

};

