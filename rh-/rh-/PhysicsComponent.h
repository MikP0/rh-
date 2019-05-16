#pragma once
#include "Component.h"
#include "ColliderTypes.h"

using namespace std;

class PhysicsComponent : public Component
{
public:
	bool isTrigger;
	shared_ptr<ColliderBase> ColliderBounding;

	PhysicsComponent();
	PhysicsComponent(ColliderType colliderType);
	PhysicsComponent(shared_ptr<ColliderBase> collider);
	~PhysicsComponent();

	virtual ComponentType GetType(void) override;
};

