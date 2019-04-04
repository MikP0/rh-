#include "pch.h"
#include "Entity.h"


Entity::Entity()
{
}


Entity::~Entity()
{
}

int Entity::getId() const
{
	return _id;
}

std::shared_ptr<Entity> Entity::getPartent() const
{
	return _parent;
}

std::vector<std::shared_ptr<Entity>> Entity::getAllChildren() const
{
	return _children;
}

Transform Entity::getTransform() const
{
	return _transform;
}

std::shared_ptr<Entity> Entity::getChildById(int id)
{
	for (auto entity : _children) {
		if (entity->getId == id)
		{
			return entity;
		}
	}
	return nullptr;
}

void Entity::addChild(std::shared_ptr<Entity> child)
{
	_children.push_back(child);
}