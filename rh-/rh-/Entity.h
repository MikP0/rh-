//
// Entity.h
//

#pragma once

#include <vector>
#include "Transform.h"
#include "World.h"
#include "pch.h"


class RenderableComponent;

enum Tags {
	NONE,
	PLAYER,
	ENEMY
};


class Entity : public std::enable_shared_from_this<Entity>
{
public:
	Entity();
	Entity(const std::string name);
	Entity(const Entity&);
	virtual ~Entity();

	std::size_t GetId() const;
	Entity* GetPartent() const;
	std::vector<std::shared_ptr<Entity>> GetAllChildren() const;
	std::shared_ptr<Transform> GetTransform() const;
	dxmath::Matrix GetWorldMatrix() const;
	std::string GetName() const;

	void SetTransform(std::shared_ptr<Transform> transform);
	void SetWorldMatrix(dxmath::Matrix matrix);
	void SetParent(Entity* parent);
	void SetName(std::string name);

	void SetActive(bool activeFlag);
	bool IsActive();


	std::shared_ptr<Entity> GetChildById(int id);

	void AddChild(std::shared_ptr<Entity> child);

	void Update();
	
	std::unique_ptr<DirectX::Model> Model; //TODO: Move to renderer component

	template<typename TComponent, typename... Args>
	void AddComponent(Args ... args) 
	{
		_world->AddComponent<TComponent, Args...>(_id, args...);
	}

	template<typename TComponent>
	void RemoveComponent() 
	{
		_world->RemoveComponent<TComponent>();
	}

	template<typename TComponent>
	std::shared_ptr<TComponent> GetComponent()
	{
		return _world->GetComponent<TComponent>(_id);
	}

	void SetWorld(std::shared_ptr<World> world);

	void SetTag(Tags tag);
	Tags GetTag();

	static std::size_t nextId;
	
private:
	std::size_t _id;
	
	std::shared_ptr<Transform> _transform;
	std::vector<std::shared_ptr<Entity>> _children;
	Entity *_parent;
	std::string _name;
	dxmath::Matrix _worldMatrix;

	std::shared_ptr<World> _world;
	bool _active;

	Tags _tag;
};

