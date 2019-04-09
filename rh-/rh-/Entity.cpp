//
// Entity.cpp
//
#include "pch.h"
#include "Entity.h"


int Entity::nextId = 0;

Entity::Entity()
{
	_transform = std::make_shared<Transform>();
	_id = ++nextId;
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

std::shared_ptr<Transform> Entity::GetTransform() const
{
	return _transform;
}

dxmath::Matrix Entity::GetWorldMatrix() const
{
	return _worldMatrix;
}

void Entity::SetTransform(std::shared_ptr<Transform> transform)
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
		if (entity->GetId() == id)
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