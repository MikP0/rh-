#pragma once

#include <list>
#include <queue> 
#include <DirectXCollision.h>
#include "PhysicsComponent.h"
#include "Collision.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

typedef shared_ptr<PhysicsComponent> PhysicsComponentPtr;
//typedef shared_ptr<ColliderSphere> ColliderSpherePtr;
typedef shared_ptr<ColliderAABB> ColliderAABBptr;
typedef shared_ptr<ColliderFrustum> ColliderFrustumPtr;
typedef shared_ptr<ColliderRay> ColliderRayPtr;
typedef shared_ptr<Collision> CollisionPtr;
typedef vector<ContainmentType> ContainmentTypeVector;

class OctTree : public enable_shared_from_this<OctTree>
{
public:
	const int MIN_SIZE = 1;

	static shared_ptr<OctTree> Root;
	static list<PhysicsComponentPtr> AllTreeObjects;

	ColliderAABBptr Region;
	list<PhysicsComponentPtr> NodeObjects;
	vector<shared_ptr<OctTree>> ChildrenNodes;
	shared_ptr<OctTree> ParentNode;
	byte ActiveNodes;
	
	OctTree(ColliderAABBptr region, list<shared_ptr<PhysicsComponent>> objList);
	OctTree(ColliderAABBptr region);
	~OctTree();

	bool HasChildren();
	bool IsRoot();

	void UnloadContent();
	void UnloadHelper(shared_ptr<OctTree> root);

	void InsertIntoTree(vector<PhysicsComponentPtr> objects);
	void BuildTree();

	shared_ptr<OctTree> CreateNode(ColliderAABBptr region, list<PhysicsComponentPtr> objList);
	shared_ptr<OctTree> CreateNode(ColliderAABBptr region, PhysicsComponentPtr item);

	list<CollisionPtr> GetIntersection(list<PhysicsComponentPtr> parentObjs);
	list<CollisionPtr> GetIntersection(ColliderRayPtr intersectRay);
	list<CollisionPtr> GetIntersection(ColliderFrustumPtr colliderFrustum);
};

