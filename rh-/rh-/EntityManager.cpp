#include "pch.h"
#include "EntityManager.h"

EntityManager::EntityManager()
{
}

int EntityManager::CreateEntity()
{
	auto newEntity = std::make_shared<Entity>();
	_entityPool.push_back(newEntity);
	return newEntity->GetId();
}

int EntityManager::CreateEntity(std::string name)
{
	auto newEntity = std::make_shared<Entity>(name);
	_entityPool.push_back(newEntity);
	return newEntity->GetId();
}

void EntityManager::DestroyEntity(int entityId) //TODO: Check if it works
{
	_entityPool.erase(
		std::remove_if(
			_entityPool.begin(),
			_entityPool.end(),
			[&](std::shared_ptr<Entity> element) -> bool {
		if (element->GetId() == entityId)
			return true;
		else
			return false;
			}
		),
		_entityPool.end()
		);
}

std::shared_ptr<Entity> EntityManager::GetEntity(int entityId)
{
	for (auto entity : _entityPool) {
		if (entity->GetId() == entityId) {
			return entity;
		}
	}
	return nullptr;
}

std::shared_ptr<Entity> EntityManager::GetEntity(std::string entityName)
{
	for (auto entity : _entityPool) {
		if (entity->GetName() == entityName) {
			return entity;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<Entity>> EntityManager::GetEntitiesWithComponent(ComponentType componentType)
{
	std::vector<std::shared_ptr<Entity>> entitiesWithComponent;

	for (auto pair : _entityComponentMap) 
	{
		if (pair.second->GetType()._name == componentType._name) {
			entitiesWithComponent.emplace_back(GetEntity(pair.first));
		}
	}

	return entitiesWithComponent;
}

void EntityManager::AddComponent(int id, std::shared_ptr<Component> component)
{
	_entityComponentMap.insert(std::make_pair(id, component));
}

void EntityManager::AddComponent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> component)
{
	_entityComponentMap.insert(std::make_pair(entity->GetId(), component));
}
