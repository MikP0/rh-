#pragma once

#include <list>
#include <queue> 
#include <DirectXCollision.h>
#include "PhysicsComponent.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

struct MyBoundingBox
{
	BoundingBox Box;
	Vector3 Min, Max;

	MyBoundingBox() {};

	MyBoundingBox(BoundingBox box) : Box(box) 
	{
		Min = Vector3(box.Center - box.Extents);
		Max = Vector3(box.Center + box.Extents);
	};

	MyBoundingBox(Vector3 min, Vector3 max) : Min(min), Max(max)
	{
		Vector3 half = Vector3(Max - Min) / 2.0f;
		Vector3 center = Min + half;

		Box.Center = center;
		Box.Extents = half;
	};

	~MyBoundingBox() {};
};

class OctTree : public enable_shared_from_this<OctTree>
{
public:
	MyBoundingBox _region;
	list<shared_ptr<PhysicsComponent>> _objects;

	/*These are items which we're waiting to insert into the data structure. 
	We want to accrue as many objects in here as possible before we inject them 
	into the tree. This is slightly more cache friendly. */
	static queue<shared_ptr<PhysicsComponent>> _pendingInsertion;

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
	OctTree(MyBoundingBox region);
	OctTree(MyBoundingBox region, list<shared_ptr<PhysicsComponent>> objList);
	~OctTree();
	
	void UpdateTree();
	void BuildTree();
	shared_ptr<OctTree> CreateNode(MyBoundingBox region, list<shared_ptr<PhysicsComponent>> objList);
	shared_ptr<OctTree> CreateNode(MyBoundingBox region, shared_ptr<PhysicsComponent> Item);
};

