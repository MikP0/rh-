#pragma once

#include <unordered_map>

#include "pch.h"
#include "Entity.h"
#include "Component.h"

class EntityManager
{
public:
	EntityManager();
	virtual ~EntityManager();

private:
	std::unordered_multimap <Entity, Component> _entityComponentMap;
};

