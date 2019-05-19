#pragma once

#include <map>

#include "Entity.h"
#include "System.h"
#include "EntityManager.h"
#include "ComponentFactory.h"

#pragma region Aliases
using EntityMap = std::map<std::size_t, std::shared_ptr<Entity>>;
#pragma endregion


/*
	TODO: Change Systems - remove EntityManager [ ]
	TODO: Complete World class [x]
	TODO: Remove ComponentFactory class - Add Components in Entity [ ]
	TODO: Add World Manager to read from XML ? [ ]
*/

class World
{
public:
	World();
	~World();

	template<typename TSystem>
	void AddSystem(std::shared_ptr<TSystem> system, size_t index);

	template <typename TSystem>
	void RemoveSystem();

	template <typename TSystem>
	bool DoesSystemExist() const;

	template <typename TSystem>
	void InitializeSystem();

	void InitializeAllSystems();

	void RemoveAllSystems();

	Entity CreateEntity();
	Entity CreateEntity(std::string name);

	void KillEntity(Entity& entity);
	void KillEntities(std::vector<Entity>& entities);

	void ActivteEntity(Entity& entity);
	void DeactivateEntity(Entity& entity);

	bool IsActivated(Entity& entity) const;

	template<typename TComponent>
	std::vector<std::shared_ptr<TComponent>> GetComponents();

	void RefreshWorld();

	void ClearWorld();

	std::size_t GetEntityCount();

	EntityMap GetAllEntities();

	std::shared_ptr<Entity> GetEntity(std::size_t index);

private:
	EntityMap _entityPoolMap;
	std::map<std::size_t, std::shared_ptr<System>> _systemPoolMap;
	std::unordered_multimap<int, std::shared_ptr<Component>> _entityComponentMap; //EntityId - Component pointer multimap

	std::shared_ptr<Entity> _sceneEntity;
	std::shared_ptr<ComponentFactory> _componentFactory;
};

