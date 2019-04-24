#pragma once
#include "System.h"
class RenderableSystem :
	public System
{
public:
	
	RenderableSystem(std::shared_ptr<EntityManager> entityManager);
	virtual ~RenderableSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;
};

