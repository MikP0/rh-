/*Note: we want to avoid allocating memory for as long as possible since 
there can be lots of nodes.*/
/// 
/// Creates an oct tree which encloses the given region and contains the provided objects. 
///

#include "pch.h"
#include "OctTree.h"

queue<shared_ptr<PhysicsComponent>> OctTree::_pendingInsertion;
bool OctTree::_treeReady = false;
bool OctTree::_treeBuilt = false;

//The bounding region for the oct tree. 
//The list of objects contained within the bounding region 
OctTree::OctTree(MyBoundingBox region, list<shared_ptr<PhysicsComponent>> objList)
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
	_region = MyBoundingBox(Vector3::Zero, Vector3::Zero);
	_curLife = -1;
}

/// 
/// Creates an octTree with a suggestion for the bounding region containing the items. 
/// 

///The suggested dimensions for the bounding region. 
///Note: if items are outside this region, the region will be automatically resized. 
OctTree::OctTree(MyBoundingBox region)
{
	_region = region;
	_objects = list<shared_ptr<PhysicsComponent>>();
	_curLife = -1;
}

OctTree::~OctTree()
{
}

void OctTree::UpdateTree() //complete & tested 
{
	if (!_treeBuilt)
	{
		while (_pendingInsertion.size() != 0)
		{
			shared_ptr<PhysicsComponent> element = _pendingInsertion.front();
			_pendingInsertion.pop();
			_objects.push_back(element);
		}
			
		BuildTree();
	}
	else
	{
		while (_pendingInsertion.size() != 0)
		{
			shared_ptr<PhysicsComponent> element = _pendingInsertion.front();
			_pendingInsertion.pop();
			//Insert(element);
		}
	}

	_treeReady = true;
}

/// 
/// Naively builds an oct tree from scratch.
/// 
void OctTree::BuildTree() //complete & tested
{
	//terminate the recursion if we're a leaf node
	if (_objects.size() <= 1)
		return;

	Vector3 dimensions = _region.Max - _region.Min;

	//Check to see if the dimensions of the box are greater than the minimum dimensions
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		return;
	}

	Vector3 center = _region.Box.Center;

	//Create subdivided regions for each octant
	shared_ptr<MyBoundingBox> octant[8];

	octant[0] = make_shared<MyBoundingBox>(_region.Min, center);
	octant[1] = make_shared<MyBoundingBox>(Vector3(center.x, _region.Min.y, _region.Min.z), Vector3(_region.Max.x, center.y, center.z));
	octant[2] = make_shared<MyBoundingBox>(Vector3(center.x, _region.Min.y, center.z), Vector3(_region.Max.x, center.y, _region.Max.z));
	octant[3] = make_shared<MyBoundingBox>(Vector3(_region.Min.x, _region.Min.y, center.z), Vector3(center.x, center.y, _region.Max.z));
	octant[4] = make_shared<MyBoundingBox>(Vector3(_region.Min.x, center.y, _region.Min.z), new Vector3(center.x, _region.Max.y, center.z));
	octant[5] = make_shared<MyBoundingBox>(Vector3(center.x, center.y, _region.Min.z), Vector3(_region.Max.x, _region.Max.y, center.z));
	octant[6] = make_shared<MyBoundingBox>(center, _region.Max);
	octant[7] = make_shared<MyBoundingBox>(Vector3(_region.Min.x, center.y, center.z), new Vector3(center.x, _region.Max.y, _region.Max.z));

}

shared_ptr<OctTree> OctTree::CreateNode(MyBoundingBox region, list<shared_ptr<PhysicsComponent>> objList) //complete & tested
{
	if (objList.size() == 0)
		return nullptr;
	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->_parent = shared_from_this();
	return ret;
}

shared_ptr<OctTree> OctTree::CreateNode(MyBoundingBox region, shared_ptr<PhysicsComponent> Item)
{
	list<shared_ptr<PhysicsComponent>> objList(1); //sacrifice potential CPU time for a smaller memory footprint
	objList.push_back(Item);
	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->_parent = shared_from_this();
	return ret;
}
