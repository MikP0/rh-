#include "pch.h"
#include "Entity.h"


Entity::Entity()
{
}


Entity::~Entity()
{
}

int Entity::GetId() const
{
	return _id;
}

std::shared_ptr<Entity> Entity::GetPartent() const
{
	return _parent;
}

std::vector<std::shared_ptr<Entity>> Entity::GetAllChildren() const
{
	return _children;
}

Transform Entity::GetTransform() const
{
	return _transform;
}

dxmath::Matrix Entity::GetWorldMatrix() const
{
	return _worldMatrix;
}

void Entity::SetTransform(Transform transform)
{
	_transform = transform;
}

void Entity::SetWorldMatrix(dxmath::Matrix matrix)
{
	_worldMatrix = matrix;
}

std::shared_ptr<Entity> Entity::GetChildById(int id)
{
	for (auto entity : _children) {
		if (entity->getId() == id)
		{
			return entity;
		}
	}
	return nullptr;
}

void Entity::AddChild(std::shared_ptr<Entity> child)
{
	_children.push_back(child);
}