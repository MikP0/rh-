//
// Entity.cpp
//
#include "pch.h"
#include "Entity.h"


std::size_t Entity::nextId = 0;

Entity::Entity()
{
	_transform = std::make_shared<Transform>();
	_id = ++nextId;
	_parent = nullptr;
	_name = "Entity " + std::to_string(_id);
	_worldMatrix = DirectX::SimpleMath::Matrix::Identity;
	_world = nullptr;
	_active = true;
	_tag = Tags::NONE;
}

Entity::Entity(const std::string name)
{
	_transform = std::make_shared<Transform>();
	_id = ++nextId;
	_parent = nullptr;
	_name = name;
	_worldMatrix = DirectX::SimpleMath::Matrix::Identity;
	_world = nullptr;
	_active = true;
	_tag = Tags::NONE;
}

Entity::Entity(const Entity & entity)
{
	_transform = entity._transform;
	_id = entity._id;
	_name = entity._name;
	_children = entity._children;
	_parent = entity._parent;
	_worldMatrix = entity._worldMatrix;
	_active = entity._active;
	_tag = entity._tag;
}

Entity::~Entity()
{
}

std::size_t Entity::GetId() const
{
	return _id;
}

Entity* Entity::GetPartent() const
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

std::string Entity::GetName() const
{
	return _name;
}

void Entity::SetTransform(std::shared_ptr<Transform> transform)
{
	_transform = transform;
}

void Entity::SetWorldMatrix(dxmath::Matrix matrix)
{
	_worldMatrix = matrix;
}

void Entity::SetParent(Entity* parent)
{
	_parent = parent;
}

void Entity::SetName(std::string name)
{
	_name = name;
}

void Entity::SetActive(bool activeFlag)
{
	_active = activeFlag;
}

bool Entity::IsActive()
{
	return _active;
}

std::shared_ptr<Entity> Entity::GetChildById(int id)
{
	for (auto entity : _children) 
	{
		if (entity->GetId() == id)
		{
			return entity;
		}
	}
	return nullptr;
}

void Entity::AddChild(std::shared_ptr<Entity> child)
{
	child->SetParent(this);
	_children.push_back(child);
}

void Entity::Update()
{
	if (_parent == nullptr)
		_worldMatrix = _transform->GetTransformMatrix();
	else
		_worldMatrix = _parent->GetWorldMatrix() * _transform->GetTransformMatrix();

	if (_children.size() != 0) {
		for (auto child : _children)
		{
			child->Update();
		}
	}
}

void Entity::SetWorld(std::shared_ptr<World> world)
{
	_world = world;
}

void Entity::SetTag(Tags tag)
{
	_tag = tag;
}

Tags Entity::GetTag()
{
	return _tag;
}

