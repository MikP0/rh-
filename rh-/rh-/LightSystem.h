#pragma once

#include "pch.h"
#include "System.h"
#include "ToonFactory.h"
#include "LightComponent.h"
#include "ShadowFactory.h"
#include "ReflectionFactory.h"

class LightSystem : public System
{
public:
	LightSystem(std::shared_ptr<ShadowFactory> ShadowsfxFactory, std::shared_ptr<ToonFactory> NoShadowsfxFactory, std::shared_ptr<ReflectionFactory> RefFactory);
	~LightSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;

	std::shared_ptr<ShadowFactory> _ShadowsfxFactory;
	std::shared_ptr<ToonFactory> _noShadowsfxFactory;
	std::shared_ptr<ReflectionFactory> _refFactory;
};

