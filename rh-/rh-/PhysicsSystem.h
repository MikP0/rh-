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

class PhysicsSystem : public System
{
public:
	PhysicsSystem(std::shared_ptr<EntityManager> entityManager);
	~PhysicsSystem();

	void UpdateCollidersPositions();
	void UpdateColliderPosition(PhysicsComponentPtr component);

	virtual std::vector<ComponentPtr> GetComponents(ComponentType componentType) override;
	virtual void UpdateComponentsCollection() override;

	void ResetAllUpdatePositionFlags();

	void InsertComponent(PhysicsComponentPtr component); // temporary function for tests

	virtual void Initialize() override;
	virtual void Iterate() override;

private:
	std::vector<PhysicsComponentPtr> _components;
};

