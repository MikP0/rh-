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
typedef shared_ptr<ColliderFrustum> ColliderFrustumPtr;
typedef shared_ptr<ColliderRay> ColliderRayPtr;
typedef std::shared_ptr<Collision> CollisionPtr;
typedef vector<ContainmentType> ContainmentTypeVector;

class OctTree : public enable_shared_from_this<OctTree>
{
public:
	/// <summary>
	/// This is the bounding box region which contains all objects within the tree node.
	/// </summary>
	ColliderAABBptr Region;

	/// <summary>
	/// This is a list of all objects within the current node of the octree.
	/// </summary>
	list<PhysicsComponentPtr> NodeObjects;

	/*These are items which we're waiting to insert into the data structure. 
	We want to accrue as many objects in here as possible before we inject them 
	into the tree. This is slightly more cache friendly. */
	static deque<PhysicsComponentPtr> PendingInsertion;

	/// <summary>
	/// This is a global list of all the objects within the octree, for easy reference.
	/// </summary>
	static list<PhysicsComponentPtr> AllTreeObjects;

	/*These are all of the possible child octants for this node in the tree.*/
	shared_ptr<OctTree> ChildrenNodes[8];

	/*This is a bitmask indicating which child nodes are actively being used. 
	It adds slightly more complexity, but is faster for performance since there 
	is only one comparison instead of 8. */
	byte ActiveNodes = 0;

	/*The minumum size for enclosing region is a 1x1x1 cube.*/
	const int MIN_SIZE = 1;

	/*this is how many frames we'll wait before deleting an empty tree branch. 
	Note that this is not a constant. The maximum lifespan doubles
	every time a node is reused, until it hits a hard coded constant of 64 */
	int MaxLifespan = 8;

	/*this is a countdown time showing how much time we have left to live*/
	int CurLife = -1;

	/*A pointer to the parent node is nice to have when we're trying to do a tree update.*/ 
	shared_ptr<OctTree> ParentNode;

	/*the tree has a few objects which need to be inserted*/
	static bool TreeReady;

	/*there is no pre - existing tree yet*/
	static bool TreeBuilt;

	/// <summary>
	/// This is a reference to the root node of the octree.
	/// </summary>
	static shared_ptr<OctTree> Root;

	static list<CollisionPtr> DetectedCollisions;

	OctTree(ColliderAABBptr region, list<shared_ptr<PhysicsComponent>> objList);
	OctTree();
	OctTree(ColliderAABBptr region);
	~OctTree();
	
	void UnloadContent();
	void UnloadHelper(shared_ptr<OctTree> root);
	void Enqueue(list<PhysicsComponentPtr> item);
	void Enqueue(PhysicsComponentPtr item);
	PhysicsComponentPtr Dequeue();
	void ProcessPendingItems();
	void UpdateTree();
	void BuildTree();
	shared_ptr<OctTree> CreateNode(ColliderAABBptr region, list<PhysicsComponentPtr> objList);
	shared_ptr<OctTree> CreateNode(ColliderAABBptr region, PhysicsComponentPtr Item);
	bool HasChildren();
	bool IsRoot();
	void Update(float time);
	bool Insert(PhysicsComponentPtr Item);
	list<CollisionPtr> GetIntersection(ColliderFrustumPtr colliderFrustum);
	list<CollisionPtr> GetIntersection(ColliderRayPtr intersectRay);
	list<CollisionPtr> GetIntersection(list<PhysicsComponentPtr> parentObjs);
};

