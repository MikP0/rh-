//
// Entity.h
//

#pragma once

#include <vector>
#include "Transform.h"
#include "pch.h"

class Entity : public std::enable_shared_from_this<Entity>
{
public:
	Entity();
	Entity(const std::string name);
	Entity(const Entity&);
	virtual ~Entity();

	int GetId() const;
	Entity* GetPartent() const;
	std::vector<std::shared_ptr<Entity>> GetAllChildren() const;
	std::shared_ptr<Transform> GetTransform() const;
	dxmath::Matrix GetWorldMatrix() const;
	std::string GetName() const;

	void SetTransform(std::shared_ptr<Transform> transform);
	void SetWorldMatrix(dxmath::Matrix matrix);
	void SetParent(Entity* parent);
	void SetName(std::string name);

	std::shared_ptr<Entity> GetChildById(int id);

	void AddChild(std::shared_ptr<Entity> child);

	void Update();
	
	std::unique_ptr<DirectX::Model> Model; //TODO: Move to renderer component

	static int nextId;
	
private:
	int _id;
	
	std::shared_ptr<Transform> _transform;
	std::vector<std::shared_ptr<Entity>> _children;
	Entity *_parent;
	std::string _name;
	dxmath::Matrix _worldMatrix;
};