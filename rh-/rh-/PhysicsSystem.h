#pragma once

#include <vector>
#include <DirectXColors.h>
#include "System.h"
#include "PhysicsComponent.h"
#include "Collision.h"

typedef std::shared_ptr<PhysicsComponent> PhysicsComponentPtr;
typedef std::shared_ptr<Collision> CollisionPtr;

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	CollisionPtr Collide(PhysicsComponentPtr collider1, PhysicsComponentPtr collider2);
	void UpdateColliders();

	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

	void InsertComponent(PhysicsComponentPtr component); // temporary function for tests

protected:
	virtual void Iterate() override;

private:
	std::vector<PhysicsComponentPtr> _components;
};

