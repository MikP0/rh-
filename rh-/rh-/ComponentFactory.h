#pragma once

#include "Component.h"
#include "EntityManager.h"

class ComponentFactory
{
public:
	ComponentFactory(std::shared_ptr<EntityManager> entityManager);
	~ComponentFactory();

	void CreateComponent(int entityId, std::shared_ptr<Component> component);
	void CreateComponent(int entityId, ComponentType componentType);
	void CreateComponent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> component);
	void CreateComponent(std::shared_ptr<Entity> entity, ComponentType componentType);


private:
	std::shared_ptr<EntityManager> _entityManager;
};

