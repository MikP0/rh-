//
// System.h
//

#pragma once
#include "pch.h"
#include "Component.h"
#include "EntityManager.h"
#include "World.h"

typedef std::shared_ptr<Component> ComponentPtr;

class System
{
public:
	System(std::shared_ptr<EntityManager> entityManager) { _entityManager = std::move(entityManager); };
	virtual ~System() {};
	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) { return std::vector<ComponentPtr>(); };
	virtual void UpdateComponentsCollection() {};
	virtual void Iterate() = 0;
	virtual void Initialize() = 0;

protected:
	ComponentType _componentsType;
	std::shared_ptr<EntityManager> _entityManager;
	std::shared_ptr<World> _world;
};
