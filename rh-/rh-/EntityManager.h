#pragma once

#include"pch.h"
#include "Entity.h"
#include "Component.h"

#include <unordered_map>

class EntityManager
{
public:
	EntityManager();
	virtual ~EntityManager() {};

	int CreateEntity(); //Returns EntityID
	void DestroyEntity(int entityId);

	std::shared_ptr<Entity> GetEntity(int entityId);
	std::shared_ptr<Entity> GetEntity(std::string entityName);

private:
	std::vector<std::shared_ptr<Entity>> _entityPool;
	std::unordered_map<int, std::shared_ptr<Component>> _entityComponentMap; //EntityId - Component pointer map
};

