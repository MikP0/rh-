#pragma once

#include <vector>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "System.h"
#include "PhysicsComponent.h"

typedef std::shared_ptr<PhysicsComponent> PhysicsComponentPtr;
typedef std::shared_ptr<Collision> CollisionPtr;

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	CollisionPtr Collide(PhysicsComponentPtr collider1, PhysicsComponentPtr collider2);

	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

protected:
	virtual void Iterate() override;

private:
	std::vector<PhysicsComponentPtr> _components;
};

