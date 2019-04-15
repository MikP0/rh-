#pragma once

#include"pch.h"
#include "Entity.h"
#include "Component.h"

#include <unordered_map>

//Singleton Class for EntityManager
class EntityManager
{

public:
	static EntityManager& GetInstance()
	{
		static EntityManager instance;
		
		return instance;
	}

	//EntityManager();
	virtual ~EntityManager() {};

	int CreateEntity(); //Returns EntityID
	int CreateEntity(std::string name);
	void DestroyEntity(int entityId);

	std::shared_ptr<Entity> GetEntity(int entityId);
	std::shared_ptr<Entity> GetEntity(std::string entityName);

	void AddComponent(int id, std::shared_ptr<Component> component);
	void AddComponent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> component);

private:
	EntityManager() {}                  
	EntityManager(EntityManager const&);  // Don't Implement
	void operator=(EntityManager const&); // Don't implement

	std::vector<std::shared_ptr<Entity>> _entityPool;
	std::unordered_map<int, std::shared_ptr<Component>> _entityComponentMap; //EntityId - Component pointer map
};

