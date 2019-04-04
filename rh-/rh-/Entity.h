#pragma once

#include <vector>
#include "Transform.h"


class Entity
{
public:
	Entity();
	virtual ~Entity();

	int GetId() const;
	std::shared_ptr<Entity> GetPartent() const;
	std::vector<std::shared_ptr<Entity>> GetAllChildren() const;
	Transform GetTransform() const;
	dxmath::Matrix GetWorldMatrix() const;

	void SetTransform(Transform transform);
	void SetWorldMatrix(dxmath::Matrix matrix);

	std::shared_ptr<Entity> GetChildById(int id);

	void AddChild(std::shared_ptr<Entity> child);
	
	std::unique_ptr<DirectX::Model> Model;

private:
	int _id;
	Transform _transform;
	std::vector<std::shared_ptr<Entity>> _children;
	std::shared_ptr<Entity> _parent;
	dxmath::Matrix _worldMatrix;

};