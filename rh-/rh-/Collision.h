#pragma once

#include <vector>
#include <DirectXColors.h>
#include "ColliderTypes.h"
#include "Entity.h"
#include "PhysicsComponent.h"


using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::shared_ptr<PhysicsComponent> PhysicsComponentPtr;
typedef std::shared_ptr<ColliderBase> ColliderBasePtr;
typedef std::shared_ptr<ColliderSphere> ColliderSpherePtr;
typedef std::shared_ptr<ColliderAABB> ColliderAABBptr;
typedef std::shared_ptr<ColliderRay> ColliderRayPtr;
typedef std::vector<ContainmentType> ContainmentTypeVector;

class Collision
{
public:
	EntityPtr OriginObject;
	EntityPtr CollidingObject;
	ContainmentType CollisionKind;
	float RayIntersectDist;

	Collision(EntityPtr origin, EntityPtr collider, ContainmentType collisionKind);
	Collision(EntityPtr origin, ContainmentType collisionKind, float rayIntersectDist);
	~Collision();

	static XMVECTORF32 Collision::GetCollisionColor(ContainmentType collisionKind);

	static ContainmentTypeVector Collide(ColliderAABBptr collider1, ColliderAABBptr collider2);
	static ContainmentTypeVector Collide(ColliderSpherePtr collider1, ColliderSpherePtr collider2);
	static ContainmentTypeVector Collide(ColliderAABBptr collider1, ColliderSpherePtr collider2);
	static ContainmentTypeVector Collide(ColliderSpherePtr collider1, ColliderAABBptr collider2);
	static float Collide(ColliderAABBptr collider, ColliderRay ray);
	static float Collide(ColliderSpherePtr collider, ColliderRay ray);
	static shared_ptr<Collision> CheckCollision(PhysicsComponentPtr component1, PhysicsComponentPtr component2);
	static shared_ptr<Collision> CheckCollision(PhysicsComponentPtr component, ColliderRay ray);
};

