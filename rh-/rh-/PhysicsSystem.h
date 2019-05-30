#pragma once

#include <vector>
#include "System.h"
#include "PhysicsComponent.h"
#include "OctTree.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

typedef shared_ptr<Entity> EntityPtr;
typedef shared_ptr<PhysicsComponent> PhysicsComponentPtr;
typedef shared_ptr<Collision> CollisionPtr;
typedef vector<ContainmentType> ContainmentTypeVector;
typedef shared_ptr<OctTree> OctTreePtr;

class PhysicsSystem : public System
{
public:
	vector<CollisionPtr> AllCollisions;

	PhysicsSystem(Vector3 sceneCenter, float cubeDimension);
	~PhysicsSystem();

	void UpdateCollidersPositions();
	void UpdateColliderPosition(PhysicsComponentPtr component);

	OctTreePtr GetOctTree();
	vector<CollisionPtr> GetCurrentCollisions();
	vector<CollisionPtr> GetCollisionsWithRay(ColliderRayPtr ray);
	vector<CollisionPtr> GetCollisionsWithFrustum(ColliderFrustumPtr frustum);
	vector<CollisionPtr> GetCollisionsForEntity(int entityID);

	bool CheckIfShouldUpdateTree();
	void ResetAllUpdatePositionFlags();

	virtual void Initialize() override;
	virtual void Iterate() override;

	vector<ColliderBasePtr> GetColliders();

private:
	Vector3 _sceneCenter;
	float _sceneCubeDimension;
	OctTreePtr _octTree;
};

