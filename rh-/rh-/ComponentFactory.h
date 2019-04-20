#pragma once

#include "Component.h"
#include "EntityManager.h"

class ComponentFactory
{
public:
	ComponentFactory();
	~ComponentFactory();

	void CreateComponent(int entityId, std::shared_ptr<Component> component);
	void CreateComponent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> component);

private:
	std::shared_ptr<EntityManager> _entityManager;
};

