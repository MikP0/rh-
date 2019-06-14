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
//typedef std::shared_ptr<ColliderSphere> ColliderSpherePtr;
typedef std::shared_ptr<ColliderAABB> ColliderAABBptr;
typedef std::shared_ptr<ColliderFrustum> ColliderFrustumPtr;
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
	Collision(shared_ptr<Collision> collision);
	~Collision();

	static XMVECTORF32 Collision::GetCollisionColor(ContainmentType collisionKind);

	static ContainmentTypeVector Collide(ColliderAABBptr collider1, ColliderAABBptr collider2);
	/*static ContainmentTypeVector Collide(ColliderSpherePtr collider1, ColliderSpherePtr collider2);
	static ContainmentTypeVector Collide(ColliderAABBptr collider1, ColliderSpherePtr collider2);
	static ContainmentTypeVector Collide(ColliderSpherePtr collider1, ColliderAABBptr collider2);*/
	static ContainmentTypeVector Collide(ColliderFrustumPtr collider1, ColliderAABBptr collider2);
	//static ContainmentTypeVector Collide(ColliderFrustumPtr collider1, ColliderSpherePtr collider2);
	static float Collide(ColliderAABBptr collider, ColliderRayPtr ray);
	//static float Collide(ColliderSpherePtr collider, ColliderRayPtr ray);
	static shared_ptr<Collision> CheckCollision(PhysicsComponentPtr component1, PhysicsComponentPtr component2);
	static shared_ptr<Collision> CheckCollision(PhysicsComponentPtr component, ColliderRayPtr ray);
};

