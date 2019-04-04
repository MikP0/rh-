#pragma once

#include "Entity.h"
#include <string>

struct ComponentType {
	std::string name;
};

class Component
{
public:
	Component();
	virtual ~Component();

	virtual ComponentType getType(void) = 0;

protected:
	std::shared_ptr<Entity> _parentEntity;
};

