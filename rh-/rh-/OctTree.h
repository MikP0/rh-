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
typedef shared_ptr<ColliderSphere> ColliderSpherePtr;
typedef shared_ptr<ColliderAABB> ColliderAABBptr;
typedef shared_ptr<ColliderRay> ColliderRayPtr;
typedef std::shared_ptr<Collision> CollisionPtr;
typedef vector<ContainmentType> ContainmentTypeVector;

class OctTree : public enable_shared_from_this<OctTree>
{
public:
	static shared_ptr<OctTree> Root;
	static list<PhysicsComponentPtr> AllTreeObjects;

	ColliderAABBptr Region;
	list<PhysicsComponentPtr> _objects;

	/*These are items which we're waiting to insert into the data structure. 
	We want to accrue as many objects in here as possible before we inject them 
	into the tree. This is slightly more cache friendly. */
	static queue<PhysicsComponentPtr> _pendingInsertion;

	/*These are all of the possible child octants for this node in the tree.*/
	shared_ptr<OctTree> _childNode[8];

	/*This is a bitmask indicating which child nodes are actively being used. 
	It adds slightly more complexity, but is faster for performance since there 
	is only one comparison instead of 8. */
	byte _activeNodes = 0;

	/*The minumum size for enclosing region is a 1x1x1 cube.*/
	const int MIN_SIZE = 1;

	/*this is how many frames we'll wait before deleting an empty tree branch. 
	Note that this is not a constant. The maximum lifespan doubles
	every time a node is reused, until it hits a hard coded constant of 64 */
	int _maxLifespan = 8;

	/*this is a countdown time showing how much time we have left to live*/
	int _curLife = -1;

	/*A pointer to the parent node is nice to have when we're trying to do a tree update.*/ 
	shared_ptr<OctTree> _parent;

	/*the tree has a few objects which need to be inserted*/
	static bool _treeReady;

	/*there is no pre - existing tree yet*/
	static bool _treeBuilt;

	OctTree();
	OctTree(ColliderAABBptr region);
	OctTree(ColliderAABBptr region, list<shared_ptr<PhysicsComponent>> objList);
	~OctTree();
	
	void UnloadContent();
	void Enqueue(list<PhysicsComponentPtr> objects);
	PhysicsComponentPtr Dequeue();
	void UpdateTree();
	void BuildTree();
	shared_ptr<OctTree> CreateNode(ColliderAABBptr region, list<PhysicsComponentPtr> objList);
	shared_ptr<OctTree> CreateNode(ColliderAABBptr region, PhysicsComponentPtr Item);
	bool HasChildren();
	bool IsRoot();
	void Update(float time);
	bool Insert(PhysicsComponentPtr Item);

};

