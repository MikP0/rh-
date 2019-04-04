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

private:
	std::unordered_map<int, std::shared_ptr<Component>> _entityComponentMap; //EntityId - Component pointer map
};

