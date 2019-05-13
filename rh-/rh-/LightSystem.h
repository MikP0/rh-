#pragma once

#include "pch.h"
#include "System.h"
#include "ToonFactory.h"
#include "LightComponent.h"

class LightSystem : public System
{
public:
	LightSystem(std::shared_ptr<EntityManager> entityManager, std::shared_ptr<ToonFactory> fxFactory);
	~LightSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;

	std::shared_ptr<ToonFactory> _fxFactory;
};

