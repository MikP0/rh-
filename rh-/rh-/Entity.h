#pragma once

#include <vector>
#include "Transform.h"


class Entity
{
public:
	Entity();
	virtual ~Entity();

private:
	int id;
	Transform transform;
	std::vector<std::shared_ptr<Entity>> children;
	std::shared_ptr<Entity> parent;
};

