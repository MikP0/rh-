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
	virtual ~Entity();

	int GetId() const;
	std::shared_ptr<Entity> GetPartent() const;
	std::vector<std::shared_ptr<Entity>> GetAllChildren() const;
	std::shared_ptr<Transform> GetTransform() const;
	dxmath::Matrix GetWorldMatrix() const;

	void SetTransform(std::shared_ptr<Transform> transform);
	void SetWorldMatrix(dxmath::Matrix matrix);
	void SetParent(std::shared_ptr<Entity> parent);

	std::shared_ptr<Entity> GetChildById(int id);

	void AddChild(std::shared_ptr<Entity> child);

	void Update();
	
	std::unique_ptr<DirectX::Model> Model;

	static int nextId;

private:
	int _id;
	std::string name;
	std::shared_ptr<Transform> _transform;
	std::vector<std::shared_ptr<Entity>> _children;
	std::shared_ptr<Entity> _parent;
	dxmath::Matrix _worldMatrix;
};