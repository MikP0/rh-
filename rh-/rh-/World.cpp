#pragma once
#include "pch.h"
#include "World.h"


World::World()
{
}


World::~World()
{
}


void World::InitializeAllSystems()
{
	for (auto const& pair : _systemPoolMap)
	{
		pair.second->Initialize();
	}
}

void World::RemoveAllSystems()
{
	_entityPoolMap.clear();
}

//Entity World::CreateEntity()
//{
//	auto entity = std::make_shared<Entity>();
//	_entityPoolMap[entity->GetId()] = entity;
//	return *entity;
//}

std::shared_ptr<Entity> World::CreateEntity(std::string name)
{
	auto entity = std::make_shared<Entity>(name);
	entity->SetWorld(shared_from_this());
	_entityPoolMap[entity->GetId()] = entity;
	return entity;
}

void World::KillEntity(std::shared_ptr<Entity> entity)
{
	_entityComponentMap.erase(entity->GetId());
	_entityPoolMap.erase(entity->GetId());
}

void World::KillEntities(std::vector<Entity>& entities)
{
	for (auto l_entity : entities)
	{
		_entityPoolMap.erase(l_entity.GetId());
	}
}

void World::ActivteEntity(Entity& entity)
{
	entity.SetActive(true);
}

void World::DeactivateEntity(Entity& entity)
{
	entity.SetActive(false);
}

bool World::IsActivated(Entity& entity) const
{
	return entity.IsActive();
}

void World::RefreshWorld()
{
	for (auto const& pair : _entityPoolMap)
	{
		pair.second->Update();
	}

	for (auto const& pair : _systemPoolMap)
	{
		pair.second->Iterate();
	}
}

void World::ClearWorld()
{
	_entityComponentMap.clear();
	_systemPoolMap.clear();
	_entityPoolMap.clear();
}

std::size_t World::GetEntityCount()
{
	std::size_t size = 0;
	for (auto const& pair : _entityPoolMap)
	{
		size++;
	}
	return size;
}

std::map<std::size_t, std::shared_ptr<Entity>> World::GetAllEntities()
{
	return _entityPoolMap;
}

std::shared_ptr<Entity> World::GetEntity(std::size_t index)
{
	return _entityPoolMap[index];
}

void World::KillAllEntities()
{
	_entityComponentMap.clear();
	_entityPoolMap.clear();
}


