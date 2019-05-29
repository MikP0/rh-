#pragma once

#include "pch.h"
#include "System.h"
#include "ToonFactory.h"
#include "LightComponent.h"
#include "ShadowFactory.h"

class LightSystem : public System
{
public:
	LightSystem(std::shared_ptr<ShadowFactory> fxFactory);
	~LightSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;

	std::shared_ptr<ShadowFactory> _fxFactory;
};

