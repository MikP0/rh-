#pragma once

#include "pch.h"
#include "System.h"
#include "Entity.h"

#include <unordered_map>
#include <string>
#include <map>
#include <memory>
#include <iostream>


/*
	TODO: Change Systems - remove EntityManager [ ]
	TODO: Complete World class [x]
	TODO: Remove ComponentFactory class - Add Components in Entity [ ]
	TODO: Add World Manager to read from XML ? [ ]
*/

class Entity;
class Component;

class World : public std::enable_shared_from_this<World>
{
public:
	World();
	~World();

	template<typename TSystem>
	void AddSystem(std::shared_ptr<TSystem> system, size_t index) 
	{
		if (std::is_base_of<System, TSystem>::value) {
			std::dynamic_pointer_cast<System>(system)->SetWorld(std::move(shared_from_this()));
			_systemPoolMap[index] = system;
		}
	}

	template <typename TSystem>
	void RemoveSystem() 
	{
		for (auto const& pair : _systemPoolMap)
		{
			if (std::is_same<*pair.second, TSystem>::value)
				_systemPoolMap.erase(pair.first);
		}
	};

	template <typename TSystem>
	bool DoesSystemExist() const 
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


	template <typename TSystem>
	void InitializeSystem()
	{
		for (auto const& pair : _systemPoolMap)
		{
			if (typeid(TSystem) == typeid(*pair.second)) {
				pair.second->Initialize();
				return;
			}
		}
	}

	void InitializeAllSystems();

	void RemoveAllSystems();

	//Entity CreateEntity();
	std::shared_ptr<Entity> CreateEntity(std::string name);

	void KillEntity(std::shared_ptr<Entity> entity);
	void KillEntities(std::vector<Entity>& entities);

	void ActivteEntity(Entity& entity);
	void DeactivateEntity(Entity& entity);

	bool IsActivated(Entity& entity) const;

	template<typename TComponent>
	std::vector<std::shared_ptr<TComponent>> GetComponents()
	{
		std::vector<std::shared_ptr<TComponent>> components;

		for (auto pair : _entityComponentMap)
		{
			if (typeid(TComponent) == typeid(*pair.second)) {
				components.push_back(std::dynamic_pointer_cast<TComponent>(pair.second));
			}
		}

		return components;
	}

	void RefreshWorld();

	void ClearWorld();

	std::size_t GetEntityCount();

	std::map<std::size_t, std::shared_ptr<Entity>> GetAllEntities();

	std::shared_ptr<Entity> GetEntity(std::size_t index);

	template<typename TComponent, typename... Args>
	void AddComponent(std::size_t entityId, Args... args)
	{
		auto component = std::make_shared<TComponent>(args...);
		std::dynamic_pointer_cast<Component>(component)->SetParent(GetEntity(entityId));
		_entityComponentMap.insert(std::make_pair(entityId, std::move(component)));
	}

	template<typename TComponent>
	void RemoveComponent(std::size_t entityId)
	{
		typedef std::unordered_multimap<std::size_t, std::shared_ptr<Component>>::iterator iterator;
		std::pair<iterator, iterator> iterpair = _entityComponentMap.equal_range(entityId);

		iterator it = iterpair.first;
		for (; it != iterpair.second; ++it) {
			if (typeid(TComponent) == typeid(*it->second))
			{
				_entityComponentMap.erase(it);
				break;
			}
		}
	}

	template<typename TComponent>
	std::shared_ptr<TComponent> GetComponent(std::size_t entityId)
	{
		typedef std::unordered_multimap<std::size_t, std::shared_ptr<Component>>::iterator iterator;
		std::pair<iterator, iterator> iterpair = _entityComponentMap.equal_range(entityId);

		iterator it = iterpair.first;
		for (; it != iterpair.second; ++it) {
			if (typeid(*it->second) == typeid(TComponent))
			{
				return std::dynamic_pointer_cast<TComponent>(it->second);
			}
		}
	}

	void KillAllEntities();

private:
	std::map<std::size_t, std::shared_ptr<Entity>> _entityPoolMap;
	std::map<std::size_t, std::shared_ptr<System>> _systemPoolMap;
	std::unordered_multimap<std::size_t, std::shared_ptr<Component>> _entityComponentMap; //EntityId - Component pointer multimap
};

