#pragma once
#include "Component.h"
#include "Collision.h"

class PhysicsComponent : public Component
{
public:
	std::shared_ptr<ColliderBase> ColliderBounding;

	PhysicsComponent();
	PhysicsComponent(ColliderType colliderType);
	~PhysicsComponent();

	virtual ComponentType GetType(void) override;
};
