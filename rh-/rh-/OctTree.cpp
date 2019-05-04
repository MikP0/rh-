/*Note: we want to avoid allocating memory for as long as possible since 
there can be lots of nodes.*/
/// 
/// Creates an oct tree which encloses the given region and contains the provided objects. 
///

#include "pch.h"
#include "OctTree.h"

bool OctTree::_treeReady = false;
bool OctTree::_treeBuilt = false;

//The bounding region for the oct tree. 
//The list of objects contained within the bounding region 
OctTree::OctTree(BoundingBox region, list<shared_ptr<PhysicsComponent>> objList)
{
	_region = region;
	_objects = objList;
	_curLife = -1;
}

OctTree::OctTree()
{
	for (int i = 0; i < 8; i++)
		_childNode[i] = make_shared<OctTree>();

	_objects = list<shared_ptr<PhysicsComponent>>();
	_region = BoundingBox(Vector3::Zero, Vector3::Zero);
	_curLife = -1;
}

/// 
/// Creates an octTree with a suggestion for the bounding region containing the items. 
/// 

///The suggested dimensions for the bounding region. 
///Note: if items are outside this region, the region will be automatically resized. 
OctTree::OctTree(BoundingBox region)
{
	_region = region;
	_objects = list<shared_ptr<PhysicsComponent>>();
	_curLife = -1;
}

OctTree::~OctTree()
{
}
