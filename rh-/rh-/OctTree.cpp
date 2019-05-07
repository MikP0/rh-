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
OctTree::OctTree(shared_ptr<ColliderAABB> region, list<shared_ptr<PhysicsComponent>> objList)
{
	_region = region;
	_objects = objList;
	_curLife = -1;
}

OctTree::OctTree()
{
	//for (int i = 0; i < 8; i++)
	//	_childNode[i] = make_shared<OctTree>();

	_objects = list<shared_ptr<PhysicsComponent>>();
	_region = make_shared<ColliderAABB>(Vector3::Zero, Vector3::Zero);
	_curLife = -1;
}

/// 
/// Creates an octTree with a suggestion for the bounding region containing the items. 
/// 

///The suggested dimensions for the bounding region. 
///Note: if items are outside this region, the region will be automatically resized. 
OctTree::OctTree(shared_ptr<ColliderAABB> region)
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

	Vector3 dimensions = _region->Max - _region->Min;

	//Check to see if the dimensions of the box are greater than the minimum dimensions
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		return;
	}

	Vector3 center = _region->Bounding.Center;

	//Create subdivided regions for each octant
	shared_ptr<ColliderAABB> octant[8];

	octant[0] = make_shared<ColliderAABB>(_region->Min, center, true);
	octant[1] = make_shared<ColliderAABB>(Vector3(center.x, _region->Min.y, _region->Min.z), Vector3(_region->Max.x, center.y, center.z), true);
	octant[2] = make_shared<ColliderAABB>(Vector3(center.x, _region->Min.y, center.z), Vector3(_region->Max.x, center.y, _region->Max.z), true);
	octant[3] = make_shared<ColliderAABB>(Vector3(_region->Min.x, _region->Min.y, center.z), Vector3(center.x, center.y, _region->Max.z), true);
	octant[4] = make_shared<ColliderAABB>(Vector3(_region->Min.x, center.y, _region->Min.z), Vector3(center.x, _region->Max.y, center.z), true);
	octant[5] = make_shared<ColliderAABB>(Vector3(center.x, center.y, _region->Min.z), Vector3(_region->Max.x, _region->Max.y, center.z), true);
	octant[6] = make_shared<ColliderAABB>(center, _region->Max, true);
	octant[7] = make_shared<ColliderAABB>(Vector3(_region->Min.x, center.y, center.z), Vector3(center.x, _region->Max.y, _region->Max.z), true);

	//This will contain all of our objects which fit within each respective octant.
	list<shared_ptr<PhysicsComponent>> octList[8];

	for (int i = 0; i < 8; i++)
		octList[i] = list<shared_ptr<PhysicsComponent>>();

	//this list contains all of the objects which got moved down the tree and can be delisted from this node.
	list<shared_ptr<PhysicsComponent>> delist;

	for each(shared_ptr<PhysicsComponent> obj in _objects)
	{
		ColliderType ColliderType = obj->ColliderBounding->Type;

		if (ColliderType == ColliderType::AABB)
		{
			shared_ptr<ColliderAABB> objBoundingBox = dynamic_pointer_cast<ColliderAABB>(obj->ColliderBounding);

			for (int a = 0; a < 8; a++)
			{
				if (octant[a]->Bounding.Contains(objBoundingBox->Bounding) == ContainmentType::CONTAINS)
				{
					octList[a].push_back(obj);
					delist.push_back(obj);
					break;
				}
			}
		}
		else if (ColliderType == ColliderType::Sphere)
		{
			shared_ptr<ColliderSphere> objBoundingSphere = dynamic_pointer_cast<ColliderSphere>(obj->ColliderBounding);

			for (int a = 0; a < 8; a++)
			{
				if (octant[a]->Bounding.Contains(objBoundingSphere->Bounding) == ContainmentType::CONTAINS)
				{
					octList[a].push_back(obj);
					delist.push_back(obj);
					break;
				}
			}
		}
	}

	//delist every moved object from this node.
	for each(shared_ptr<PhysicsComponent> obj in delist)
	{
		_objects.remove(obj);
	}

	//Create child nodes where there are items contained in the bounding region
	for (int a = 0; a < 8; a++)
	{
		if (octList[a].size() != 0)
		{
			_childNode[a] = CreateNode(octant[a], octList[a]);
			_activeNodes |= (byte)(1 << a);
			_childNode[a]->BuildTree();
		}
	}

	_treeBuilt = true;
	_treeReady = true;

}

shared_ptr<OctTree> OctTree::CreateNode(shared_ptr<ColliderAABB> region, list<shared_ptr<PhysicsComponent>> objList) //complete & tested
{
	if (objList.size() == 0)
		return nullptr;
	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->_parent = shared_from_this();
	return ret;
}

shared_ptr<OctTree> OctTree::CreateNode(shared_ptr<ColliderAABB> region, shared_ptr<PhysicsComponent> Item)
{
	list<shared_ptr<PhysicsComponent>> objList(1); //sacrifice potential CPU time for a smaller memory footprint
	objList.push_back(Item);
	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->_parent = shared_from_this();
	return ret;
}

bool OctTree::HasChildren()
{
	bool hasChildren = false;

	for (int i = 0; i < 8; i++)
	{
		if (_childNode[i] != nullptr)
		{
			hasChildren = true;
			break;
		}
	}

	return hasChildren;
}

void OctTree::Update(float time)
{
	if (_treeBuilt == true && _treeReady == true)
	{
		//Start a count down death timer for any leaf nodes which don't have objects or children.
		//when the timer reaches zero, we delete the leaf. If the node is reused before death, we double its lifespan.
		//this gives us a "frequency" usage score and lets us avoid allocating and deallocating memory unnecessarily
		if (_objects.size() == 0)
		{
			bool hasChildren;
			int childrenAmount = 0;

			for (int i = 0; i < 8; i++)
			{
				if (_childNode != nullptr)
					childrenAmount++;
			}

			if (HasChildren() == false)
			{
				if (_curLife == -1)
					_curLife = _maxLifespan;
				else if (_curLife > 0)
				{
					_curLife--;
				}
			}
		}
		else
		{
			if (_curLife != -1)
			{
				if (_maxLifespan <= 64)
					_maxLifespan *= 2;
				_curLife = -1;
			}
		}

		list<shared_ptr<PhysicsComponent>> movedObjects(_objects.size());
		
		//go through and update every object in the current tree node
		for each(shared_ptr<PhysicsComponent> gameObj in _objects)
		{
			//we should figure out if an object actually moved so that we know whether we need to update this node in the tree.
			if (gameObj->GetParent()->GetTransform()->GetUpdatedMoveFlag() == true)
			{
				movedObjects.push_back(gameObj);
			}
		}

		//prune any dead objects from the tree.
		for each(shared_ptr<PhysicsComponent> object in _objects)
		{
			if (!object->CheckIfEnabled() == false)
			{
				list<shared_ptr<PhysicsComponent>>::iterator it;
				it = std::find(movedObjects.begin(), movedObjects.end(), object);

				if (it != movedObjects.end())
					movedObjects.erase(it);

				_objects.remove(object);
			}
		}
		
		// DOKONCZYC ! 

	}
}
