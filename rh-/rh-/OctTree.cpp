/*Note: we want to avoid allocating memory for as long as possible since 
there can be lots of nodes.*/
/// 
/// Creates an oct tree which encloses the given region and contains the provided objects. 
///

#include "pch.h"
#include "OctTree.h"

shared_ptr<OctTree> OctTree::Root = nullptr;
list<PhysicsComponentPtr> OctTree::AllTreeObjects = list<PhysicsComponentPtr>();
queue<PhysicsComponentPtr> OctTree::_pendingInsertion;
bool OctTree::_treeReady = false;
bool OctTree::_treeBuilt = false;

//The bounding region for the oct tree. 
//The list of objects contained within the bounding region 
OctTree::OctTree(ColliderAABBptr region, list<PhysicsComponentPtr> objList)
{
	Region = region;
	_objects = objList;
	_curLife = -1;
}

OctTree::OctTree()
{
	//for (int i = 0; i < 8; i++)
	//	_childNode[i] = make_shared<OctTree>();

	_objects = list<PhysicsComponentPtr>();
	Region = make_shared<ColliderAABB>(Vector3::Zero, Vector3::Zero);
	_curLife = -1;
}

/// 
/// Creates an octTree with a suggestion for the bounding region containing the items. 
/// 

///The suggested dimensions for the bounding region. 
///Note: if items are outside this region, the region will be automatically resized. 
OctTree::OctTree(ColliderAABBptr region)
{
	Region = region;
	_objects = list<PhysicsComponentPtr>();
	_curLife = -1;
}

OctTree::~OctTree()
{
}

void OctTree::UnloadContent()
{
	_objects = list<PhysicsComponentPtr>();

	for (int i = 0; i < 8; i++)
		_childNode[i].reset();
}

void OctTree::Enqueue(list<PhysicsComponentPtr> objects)
{
	for each(PhysicsComponentPtr object in objects)
		_pendingInsertion.push(object);
}

PhysicsComponentPtr OctTree::Dequeue()
{
	PhysicsComponentPtr element = _pendingInsertion.front();
	_pendingInsertion.pop();

	return element;
}

void OctTree::UpdateTree() //complete & tested 
{
	if (!_treeBuilt)
	{
		while (_pendingInsertion.size() != 0)
		{
			PhysicsComponentPtr element = Dequeue();
			_objects.push_back(element);
			AllTreeObjects.push_back(element);
		}
			
		BuildTree();
	}
	else
	{
		while (_pendingInsertion.size() != 0)
		{
			PhysicsComponentPtr element = Dequeue();
			Insert(element);
			AllTreeObjects.push_back(element);
		}
	}

	_treeReady = true;
}

/// 
/// Naively builds an oct tree from scratch.
/// 
void OctTree::BuildTree() //complete & tested
{
	if (IsRoot())
	{
		Root = shared_from_this();
	}

	//terminate the recursion if we're a leaf node
	if (_objects.size() <= 1)
		return;

	Vector3 dimensions = Region->Max - Region->Min;

	//Check to see if the dimensions of the box are greater than the minimum dimensions
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		return;
	}

	Vector3 center = Region->Bounding.Center;

	//Create subdivided regions for each octant
	ColliderAABBptr octant[8];

	octant[0] = make_shared<ColliderAABB>(Region->Min, center, true);
	octant[1] = make_shared<ColliderAABB>(Vector3(center.x, Region->Min.y, Region->Min.z), Vector3(Region->Max.x, center.y, center.z), true);
	octant[2] = make_shared<ColliderAABB>(Vector3(center.x, Region->Min.y, center.z), Vector3(Region->Max.x, center.y, Region->Max.z), true);
	octant[3] = make_shared<ColliderAABB>(Vector3(Region->Min.x, Region->Min.y, center.z), Vector3(center.x, center.y, Region->Max.z), true);
	octant[4] = make_shared<ColliderAABB>(Vector3(Region->Min.x, center.y, Region->Min.z), Vector3(center.x, Region->Max.y, center.z), true);
	octant[5] = make_shared<ColliderAABB>(Vector3(center.x, center.y, Region->Min.z), Vector3(Region->Max.x, Region->Max.y, center.z), true);
	octant[6] = make_shared<ColliderAABB>(center, Region->Max, true);
	octant[7] = make_shared<ColliderAABB>(Vector3(Region->Min.x, center.y, center.z), Vector3(center.x, Region->Max.y, Region->Max.z), true);

	//This will contain all of our objects which fit within each respective octant.
	list<PhysicsComponentPtr> octList[8];
	
	for (int i = 0; i < 8; i++)
		octList[i] = list<PhysicsComponentPtr>();

	//this list contains all of the objects which got moved down the tree and can be delisted from this node.
	list<PhysicsComponentPtr> delist;

	for each(PhysicsComponentPtr obj in _objects)
	{
		ColliderType colliderType = obj->ColliderBounding->Type;

		if (colliderType == ColliderType::AABB)
		{
			ColliderAABBptr objBoundingBox = dynamic_pointer_cast<ColliderAABB>(obj->ColliderBounding);

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
		else if (colliderType == ColliderType::Sphere)
		{
			ColliderSpherePtr objBoundingSphere = dynamic_pointer_cast<ColliderSphere>(obj->ColliderBounding);

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
	for each(PhysicsComponentPtr obj in delist)
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

shared_ptr<OctTree> OctTree::CreateNode(ColliderAABBptr region, list<PhysicsComponentPtr> objList) //complete & tested
{
	if (objList.size() == 0)
		return nullptr;
	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->_parent = shared_from_this();
	return ret;
}

shared_ptr<OctTree> OctTree::CreateNode(ColliderAABBptr region, PhysicsComponentPtr Item)
{
	list<PhysicsComponentPtr> objList(1); //sacrifice potential CPU time for a smaller memory footprint
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

bool OctTree::IsRoot()
{
	if (_parent == nullptr)
		return true;
	else
		return false;
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

		list<PhysicsComponentPtr> movedObjects(_objects.size());
		
		//go through and update every object in the current tree node
		for each(PhysicsComponentPtr gameObj in _objects)
		{
			//we should figure out if an object actually moved so that we know whether we need to update this node in the tree.
			if (gameObj->GetParent()->GetTransform()->GetUpdatedMoveFlag() == true)
			{
				movedObjects.push_back(gameObj);
			}
		}

		//prune any dead objects from the tree.
		for each(PhysicsComponentPtr object in _objects)
		{
			if (!object->CheckIfEnabled() == false)
			{
				list<PhysicsComponentPtr>::iterator it;
				it = std::find(movedObjects.begin(), movedObjects.end(), object);

				if (it != movedObjects.end())
					movedObjects.erase(it);

				_objects.remove(object);
				AllTreeObjects.remove(object);
			}
		}
		
		//prune out any dead branches in the tree
		for (int flags = _activeNodes, index = 0; flags > 0; flags >>= 1, index++)
			if ((flags & 1) == 1 && _childNode[index]->_curLife == 0)
			{
				if (_childNode[index]->_objects.size() > 0)
				{
					//throw new Exception("Tried to delete a used branch!");
					_childNode[index]->_curLife = -1;
				}
				else
				{
					_childNode[index] = nullptr;
					_activeNodes ^= (byte)(1 << index);       //remove the node from the active nodes flag list
				}
			}

		//recursively update any child nodes.
		for (int flags = _activeNodes, index = 0; flags > 0; flags >>= 1, index++)
		{
			if ((flags & 1) == 1)
			{
				if (_childNode != nullptr && _childNode[index] != nullptr)
					_childNode[index]->Update(time);
			}
		}

		//If an object moved, we can insert it into the parent and that will insert it into the correct tree node.
		//note that we have to do this last so that we don't accidentally update the same object more than once per frame.
		for each(PhysicsComponentPtr movedObj in movedObjects)
		{
			shared_ptr<OctTree> current = shared_from_this();
			//figure out how far up the tree we need to go to reinsert our moved object
			//we are either using a bounding rect or a bounding sphere
			//try to move the object into an enclosing parent node until we've got full containment
			ColliderType colliderType = movedObj->ColliderBounding->Type;

			if (colliderType == ColliderType::AABB)
			{
				ColliderAABBptr objBoundingBox = dynamic_pointer_cast<ColliderAABB>(movedObj->ColliderBounding);

				while (current->Region->Bounding.Contains(objBoundingBox->Bounding) != ContainmentType::CONTAINS)
					if (current->_parent != nullptr) 
						current = current->_parent;
					else
					{
						break; //prevent infinite loops when we go out of bounds of the root node region
					}
			}
			else
				if (colliderType == ColliderType::Sphere)
				{
					ColliderSpherePtr objBoundingSphere = dynamic_pointer_cast<ColliderSphere>(movedObj->ColliderBounding);
					ContainmentType ct = current->Region->Bounding.Contains(objBoundingSphere->Bounding);

					while (ct != ContainmentType::CONTAINS)//we must be using a bounding sphere, so check for its containment.
					{
						if (current->_parent != nullptr)
						{
							current = current->_parent;
						}
						else
						{
							//the root region cannot contain the object, so we need to completely rebuild the whole tree.
							//The rarity of this event is rare enough where we can afford to take all objects out of the existing tree and rebuild the entire thing.
							list<PhysicsComponentPtr> tmp = Root->AllTreeObjects;
							Root->UnloadContent();
							Enqueue(tmp);//add to pending queue


							return;
						}

						ct = current->Region->Bounding.Contains(objBoundingSphere->Bounding);
					}
				}

				//now, remove the object from the current node and insert it into the current containing node.
				_objects.remove(movedObj);
				current->Insert(movedObj);   //this will try to insert the object as deep into the tree as we can go.

		}

		if (IsRoot())
		{
			//This will recursively gather up all collisions and create a list of them.
			//this is simply a matter of comparing all objects in the current root node with all objects in all child nodes.
			//note: we can assume that every collision will only be between objects which have moved.
			//note 2: An explosion can be centered on a point but grow in size over time. In this case, you'll have to override the update method for the explosion.
			/*list<IntersectionRecord> irList = GetIntersection(list<PhysicsComponentPtr>());

			for each(IntersectionRecord ir in irList)
			{
				if (ir.PhysicalObject != nullptr)
					ir.PhysicalObject.HandleIntersection(ir);
				if (ir.OtherPhysicalObject != nullptr)
					ir.OtherPhysicalObject.HandleIntersection(ir);
			}*/
		}
	}//end if tree built
	else
	{
		if (_pendingInsertion.size() > 0)
		{
			UpdateTree();
			Update(time);   //try this again...
		}
	}
}

/// <summary>
/// A tree has already been created, so we're going to try to insert an item into the tree without rebuilding the whole thing
/// </summary>
/// <param name="Item">The physical object to insert into the tree</param>

bool OctTree::Insert(PhysicsComponentPtr Item)
{
	/*if the current node is an empty leaf node, just insert and leave it.*/
	//if (m_objects.Count == 0 && m_activeNodes == 0)
	if (AllTreeObjects.size() == 0)
	{
		_objects.push_back(Item);
		return true;
	}

	//Check to see if the dimensions of the box are greater than the minimum dimensions.
	//If we're at the smallest size, just insert the item here. We can't go any lower!
	Vector3 dimensions = Region->Max - Region->Min;
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		_objects.push_back(Item);
		return true;
	}

	//The object won't fit into the current region, so it won't fit into any child regions.
	//therefore, try to push it up the tree. If we're at the root node, we need to resize the whole tree.
	ColliderType colliderType = Item->ColliderBounding->Type;
	ColliderAABBptr objBoundingBox;
	ColliderSpherePtr objBoundingSphere;

	if (colliderType == ColliderType::AABB)
	{
		objBoundingBox = dynamic_pointer_cast<ColliderAABB>(Item->ColliderBounding);
	}
	else
	{
		objBoundingSphere = dynamic_pointer_cast<ColliderSphere>(Item->ColliderBounding);
	}
	
	if (objBoundingBox != nullptr && Region->Bounding.Contains(objBoundingBox->Bounding) != ContainmentType::CONTAINS)
	{
		if (_parent != nullptr)
			return _parent->Insert(Item);
		else
			return false;
	}
	else
	{
		if (objBoundingSphere != nullptr && Region->Bounding.Contains(objBoundingSphere->Bounding) != ContainmentType::CONTAINS)
		{
			if (_parent != nullptr)
				return _parent->Insert(Item);
			else
				return false;
		}
	}

	//At this point, we at least know this region can contain the object but there are child nodes. Let's try to see if the object will fit
	//within a subregion of this region.

	Vector3 center = Region->Bounding.Center;

	//Find or create subdivided regions for each octant in the current region
	ColliderAABBptr childOctant[8];
	childOctant[0] = (_childNode[0] != nullptr) ? _childNode[0]->Region : make_shared<ColliderAABB>(Region->Min, center, true);
	childOctant[1] = (_childNode[1] != nullptr) ? _childNode[1]->Region : make_shared<ColliderAABB>(Vector3(center.x, Region->Min.y, Region->Min.z), Vector3(Region->Max.x, center.y, center.z));
	childOctant[2] = (_childNode[2] != nullptr) ? _childNode[2]->Region : make_shared<ColliderAABB>(Vector3(center.x, Region->Min.y, center.z), Vector3(Region->Max.x, center.y, Region->Max.z));
	childOctant[3] = (_childNode[3] != nullptr) ? _childNode[3]->Region : make_shared<ColliderAABB>(Vector3(Region->Min.x, Region->Min.y, center.z), Vector3(center.x, center.y, Region->Max.z));
	childOctant[4] = (_childNode[4] != nullptr) ? _childNode[4]->Region : make_shared<ColliderAABB>(Vector3(Region->Min.x, center.y, Region->Min.z), Vector3(center.x, Region->Max.y, center.z));
	childOctant[5] = (_childNode[5] != nullptr) ? _childNode[5]->Region : make_shared<ColliderAABB>(Vector3(center.x, center.y, Region->Min.z), Vector3(Region->Max.x, Region->Max.y, center.z));
	childOctant[6] = (_childNode[6] != nullptr) ? _childNode[6]->Region : make_shared<ColliderAABB>(center, Region->Max);
	childOctant[7] = (_childNode[7] != nullptr) ? _childNode[7]->Region : make_shared<ColliderAABB>(Vector3(Region->Min.x, center.y, center.z), Vector3(center.x, Region->Max.y, Region->Max.z));

	//First, is the item completely contained within the root bounding box?
	//note2: I shouldn't actually have to compensate for this. If an object is out of our predefined bounds, then we have a problem/error.
	//Wrong. Our initial bounding box for the terrain is constricting its height to the highest peak. Flying units will be above that.
	//Fix: I resized the enclosing box to 256x256x256. This should be sufficient.
	if (colliderType == ColliderType::AABB && Region->Bounding.Contains(objBoundingBox->Bounding) == ContainmentType::CONTAINS)
	{
		bool found = false;
		//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
		for (int a = 0; a < 8; a++)
		{
			//is the object fully contained within a quadrant?
			if (childOctant[a]->Bounding.Contains(objBoundingBox->Bounding) == ContainmentType::CONTAINS)
			{
				if (_childNode[a] != nullptr)
				{
					return _childNode[a]->Insert(Item);   //Add the item into that tree and let the child tree figure out what to do with it
				}
				else
				{
					_childNode[a] = CreateNode(childOctant[a], Item);   //create a new tree node with the item
					_activeNodes |= (byte)(1 << a);
				}
				found = true;
			}
		}

		//we couldn't fit the item into a smaller box, so we'll have to insert it in this region
		if (!found)
		{
			_objects.push_back(Item);
			return true;
		}
	} else
		if (colliderType == ColliderType::Sphere && Region->Bounding.Contains(objBoundingSphere->Bounding) == ContainmentType::CONTAINS)
		{
			bool found = false;
			//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
			for (int a = 0; a < 8; a++)
			{
				//is the object contained within a child quadrant?
				if (childOctant[a]->Bounding.Contains(objBoundingSphere->Bounding) == ContainmentType::CONTAINS)
				{
					if (_childNode[a] != nullptr)
					{
						return _childNode[a]->Insert(Item);   //Add the item into that tree and let the child tree figure out what to do with it
					}
					else
					{
						_childNode[a] = CreateNode(childOctant[a], Item);   //create a new tree node with the item
						_activeNodes |= (byte)(1 << a);
					}
					found = true;
				}
			}

			//we couldn't fit the item into a smaller box, so we'll have to insert it in this region
			if (!found)
			{
				_objects.push_back(Item);
				return true;
			}
		}

	//either the item lies outside of the enclosed bounding box or it is intersecting it. Either way, we need to rebuild
	//the entire tree by enlarging the containing bounding box
	return false;
}

list<CollisionPtr> OctTree::GetIntersection(ColliderFrustumPtr colliderFrustum)
{
	DirectX::BoundingFrustum frustum = colliderFrustum->Bounding;

	if (!_treeBuilt) 
		return list<CollisionPtr>();

	if (_objects.size() == 0 && HasChildren() == false)   //terminator for any recursion
		return list<CollisionPtr>();

	list<CollisionPtr> ret = list<CollisionPtr>();

	//test each object in the list for intersection
	for each(PhysicsComponentPtr obj in _objects)
	{
		//test for intersection
		CollisionPtr ir = Collision::CheckCollision(make_shared<PhysicsComponent>(colliderFrustum), obj);
		if (ir != nullptr)
			ret.push_back(ir);
	}

	//test each object in the list for intersection
	for (int a = 0; a < 8; a++)
	{
		if (_childNode[a] != nullptr && (frustum.Contains(_childNode[a]->Region->Bounding) == ContainmentType::INTERSECTS || frustum.Contains(_childNode[a]->Region->Bounding) == ContainmentType::CONTAINS))
		{
			list<CollisionPtr> hitList = _childNode[a]->GetIntersection(colliderFrustum);

			if (!hitList.empty()) 
				ret.merge(hitList);
		}
	}
	return ret;
}

/// <summary>
/// Gives you a list of intersection records for all objects which intersect with the given ray
/// </summary>
/// <param name="intersectRay">The ray to intersect objects against</param>
/// <returns>A list of all intersections</returns>
list<CollisionPtr> OctTree::GetIntersection(ColliderRayPtr intersectRay)
{
	if (!_treeBuilt)
		return list<CollisionPtr>();

	if (_objects.size() == 0 && HasChildren() == false)   //terminator for any recursion
		return list<CollisionPtr>();

	list<CollisionPtr> ret = list<CollisionPtr>();

	//the ray is intersecting this region, so we have to check for intersection with all of our contained objects and child regions.

	//test each object in the list for intersection
	for each(PhysicsComponentPtr obj in _objects)
	{
		CollisionPtr ir = Collision::CheckCollision(obj, intersectRay);
		if (ir != nullptr)
			ret.push_back(ir);
	}

	// test each child octant for intersection
	for (int a = 0; a < 8; a++)
	{
		if (_childNode[a] != nullptr && Collision::CheckCollision(make_shared<PhysicsComponent>(_childNode[a]->Region), intersectRay) != nullptr)
		{
			list<CollisionPtr> hits = _childNode[a]->GetIntersection(intersectRay);

			if (!hits.empty())
			{
				ret.merge(hits);
			}
		}
	}

	return ret;
}

