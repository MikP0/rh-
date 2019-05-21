#include "pch.h"
#include "World.h"


World::World()
{
}


World::~World()
{
}

template<typename TSystem>
void World::AddSystem(std::shared_ptr<TSystem> system, size_t index) 
{
	if (std::is_base_of<System, TSystem>::value) {
		_systemPoolMap[index] = system;
	}
}

template <typename TSystem>
void World::RemoveSystem()
{
	for (auto const& pair : _systemPoolMap)
	{
		if (std::is_same<*pair.second, TSystem>::value) 
			_systemPoolMap.erase(pair.first);	
	}
}

template <typename TSystem>
bool World::DoesSystemExist() const
{
	for (auto system : _systemPoolMap)
	{
		for (auto const& pair : _systemPoolMap)
		{
			if (std::is_same<*pair.second, TSystem>::value)
				return true;
		}
	}
	return false;
}

template<typename TSystem>
void World::InitializeSystem()
{
	for (auto const& pair : _systemPoolMap)
	{
		if (std::is_same<*pair.second, TSystem>::value) {
			pair.second->Initialize();
			return;
		}
	}
}

template<typename TComponent>
std::vector<std::shared_ptr<TComponent>> World::GetComponents()
{
	std::vector<std::shared_ptr<Component>> components;

	for (auto pair : _entityComponentMap)
	{
		if (std::is_same<*pair.second, TComponent>::value) {
			components.emplace_back(std::move(pair.second));
		}
	}

	return components;
}

template<typename TComponent, typename ...Args>
void World::AddComponent(std::size_t entityId, Args ...args)
{
	std::shared_ptr<TComponent> component = std::make_shared<TComponent>(args...);
	_entityComponentMap.insert(std::make_pair(entityId, std::move(component)));
}

template<typename TComponent>
void World::RemoveComponent(std::size_t entityId)
{
	typedef std::unordered_multimap<std::size_t, std::shared_ptr<Component>>::iterator iterator;
	std::pair<iterator, iterator> iterpair = _entityComponentMap.equal_range(entityId);

	iterator it = iterpair.first;
	for (; it != iterpair.second; ++it) {
		if (std::is_same<*it->second, TComponent>::value) 
		{
			_entityComponentMap.erase(it);
			break;
		}
	}
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

Entity World::CreateEntity()
{
	auto entity = std::make_shared<Entity>();
	_entityPoolMap[entity->GetId()] = entity;
	return *entity;
}

Entity World::CreateEntity(std::string name)
{
	auto entity = std::make_shared<Entity>(name);
	_entityPoolMap[entity->GetId()] = entity;
	return *entity;
}

void World::KillEntity(Entity& entity)
{
	_entityPoolMap.erase(entity.GetId());
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
	//TODO: Update entity

	for (auto const& pair : _systemPoolMap)
	{
		pair.second->Iterate();
	}
}

void World::ClearWorld()
{
	_systemPoolMap.clear();
	_entityComponentMap.clear();
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


