#pragma once

#include <vector>
#include <DirectXColors.h>
#include "System.h"
#include "PhysicsComponent.h"
#include "Collision.h"

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::shared_ptr<PhysicsComponent> PhysicsComponentPtr;
typedef std::shared_ptr<Collision> CollisionPtr;
typedef std::shared_ptr<ColliderBase> ColliderBasePtr;
typedef std::shared_ptr<ColliderSphere> ColliderSpherePtr;
typedef std::shared_ptr<ColliderAABB> ColliderAABBptr;
typedef std::shared_ptr<ColliderRay> ColliderRayPtr;
typedef std::vector<ContainmentType> ContainmentTypeVector;

class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();

	ContainmentTypeVector Collide(ColliderAABBptr collider1, ColliderAABBptr collider2);
	ContainmentTypeVector Collide(ColliderSpherePtr collider1, ColliderSpherePtr collider2);
	ContainmentTypeVector Collide(ColliderAABBptr collider1, ColliderSpherePtr collider2);
	ContainmentTypeVector Collide(ColliderSpherePtr collider1, ColliderAABBptr collider2);
	float Collide(ColliderAABBptr collider, ColliderRay ray);
	float Collide(ColliderSpherePtr collider, ColliderRay ray);

	CollisionPtr CheckCollision(PhysicsComponentPtr component1, PhysicsComponentPtr component2);
	CollisionPtr CheckCollision(PhysicsComponentPtr component, ColliderRay ray);

	void UpdateCollidersPositions();
	void UpdateColliderPosition(PhysicsComponentPtr component);

	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

	void InsertComponent(PhysicsComponentPtr component); // temporary function for tests

	virtual void Iterate() override;

private:
	std::vector<PhysicsComponentPtr> _components;
};

