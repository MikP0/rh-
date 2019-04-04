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

	virtual ComponentType GetType(void) = 0;
	std::shared_ptr<Entity> GetParent() const;

	void SetParent(std::shared_ptr<Entity> parent);


protected:
	std::shared_ptr<Entity> _parentEntity;
};

