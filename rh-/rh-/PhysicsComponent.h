#pragma once
#include "Component.h"
#include "Collision.h"

class PhysicsComponent : public Component
{
public:
	ColliderAABB CollisionBox;

	PhysicsComponent();
	~PhysicsComponent();

	virtual ComponentType GetType(void) override;
};

