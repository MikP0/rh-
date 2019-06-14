#pragma once
#include "Component.h"
#include "ColliderTypes.h"

using namespace std;

class PhysicsComponent : public Component
{
public:
	bool IsTriggered;
	shared_ptr<ColliderBase> ColliderBounding;

	PhysicsComponent(XMFLOAT3 extents);
	PhysicsComponent(Vector3 positionOffset, XMFLOAT3 extents);
	PhysicsComponent(XMFLOAT3 extents, bool isTriggered);
	PhysicsComponent(Vector3 positionOffset, XMFLOAT3 extents, bool isTriggered);
	/*PhysicsComponent(float radius);
	PhysicsComponent(Vector3 positionOffset, float radius);
	PhysicsComponent(float radius, bool isTriggered);
	PhysicsComponent(Vector3 positionOffset, float radius, bool isTriggered);*/
	PhysicsComponent(shared_ptr<ColliderBase> collider);
	PhysicsComponent(shared_ptr<ColliderBase> collider, bool isTriggered);
	~PhysicsComponent();
};

