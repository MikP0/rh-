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
	PhysicsSystem(Vector3 sceneCenter, float cubeDimension);
	~PhysicsSystem();

	void UpdateCollidersPositions();
	void UpdateColliderPosition(PhysicsComponentPtr component);

	std::vector<PhysicsComponentPtr> GetComponents(ComponentType componentType);
	virtual void UpdateComponentsCollection() override;

	void ResetAllUpdatePositionFlags();

	void InsertComponent(PhysicsComponentPtr component); // temporary function for tests

	virtual void Initialize() override;
	virtual void Iterate() override;

private:
	vector<PhysicsComponentPtr> _components;
	Vector3 _sceneCenter;
	float _sceneCubeDimension;
	OctTreePtr _octTree;
};

