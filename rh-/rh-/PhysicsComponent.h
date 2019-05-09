#pragma once
#include "Component.h"
#include "ColliderTypes.h"


class PhysicsComponent : public Component
{
public:
	bool isTrigger;
	std::shared_ptr<ColliderBase> ColliderBounding;

	PhysicsComponent();
	PhysicsComponent(ColliderType colliderType);
	~PhysicsComponent();

	virtual ComponentType GetType(void) override;
};

