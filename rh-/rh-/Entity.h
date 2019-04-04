#pragma once

#include <vector>
#include "Transform.h"


class Entity
{
public:
	Entity();
	virtual ~Entity();

	int getId() const;
	std::shared_ptr<Entity> getPartent() const;
	std::vector<std::shared_ptr<Entity>> getAllChildren() const;
	Transform getTransform() const;
	void SetTransform(Transform transform);

	std::shared_ptr<Entity> getChildById(int id);

	void addChild(std::shared_ptr<Entity> child);
	
	std::unique_ptr<DirectX::Model> Model;

private:
	int _id;
	Transform _transform;
	std::vector<std::shared_ptr<Entity>> _children;
	std::shared_ptr<Entity> _parent;
};