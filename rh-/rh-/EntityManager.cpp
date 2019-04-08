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
