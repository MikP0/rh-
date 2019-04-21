#pragma once

#include "Component.h"

class RenderableComponent : public Component
{
public:
	RenderableComponent();
	virtual ~RenderableComponent();

	virtual ComponentType GetType(void) override;

private: 
	std::unique_ptr<DirectX::Model> _model;
};

