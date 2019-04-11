#pragma once
#include "Component.h"

enum ColliderType
{
	Sphere = 0,
	AABB = 1,
	OBB = 2
};

class PhysicsComponent : public Component
{
public:
	ColliderType ColliderType;

	PhysicsComponent();
	PhysicsComponent(int colliderType);
	~PhysicsComponent();

	virtual ComponentType GetType(void) override;
};

