/*Note: we want to avoid allocating memory for as long as possible since 
there can be lots of nodes.*/
/// 
/// Creates an oct tree which encloses the given region and contains the provided objects. 
///

#include "pch.h"
#include "OctTree.h"

shared_ptr<OctTree> OctTree::Root = nullptr;
list<PhysicsComponentPtr> OctTree::AllTreeObjects = list<PhysicsComponentPtr>();
deque<PhysicsComponentPtr> OctTree::PendingInsertion;
bool OctTree::TreeReady = false;
bool OctTree::TreeBuilt = false;
list<CollisionPtr> OctTree::DetectedCollisions = list<CollisionPtr>();

/*Note: we want to avoid allocating memory for as long as possible since there can be lots of nodes.*/
/// <summary>
/// Creates an oct tree which encloses the given region and contains the provided objects.
/// </summary>
/// <param name="region">The bounding region for the oct tree.</param>
/// <param name="objList">The list of objects contained within the bounding region</param>
OctTree::OctTree(ColliderAABBptr region, list<PhysicsComponentPtr> objList)
{
	Region = region;
	NodeObjects = objList;
	CurLife = -1;
}

/// <summary>
/// Creates an Octree which is ready for object insertion. The dimensions of the octree will scale to enclose all inserted objects.
/// </summary>
OctTree::OctTree()
{
	//for (int i = 0; i < 8; i++)
	//	_childNode[i] = make_shared<OctTree>();

	NodeObjects = list<PhysicsComponentPtr>();
	Region = make_shared<ColliderAABB>(Vector3::Zero, Vector3::Zero);
	CurLife = -1;
}

/// <summary>
/// Creates an octTree with a suggestion for the bounding region containing the items.
/// </summary>
/// <param name="region">The suggested dimensions for the bounding region. 
/// Note: if items are outside this region, the region will be automatically resized.</param>
OctTree::OctTree(ColliderAABBptr region)
{
	Region = region;
	NodeObjects = list<PhysicsComponentPtr>();
	CurLife = -1;
}

OctTree::~OctTree()
{
}

void OctTree::UnloadContent()
{
	NodeObjects = list<PhysicsComponentPtr>();

	for (int i = 0; i < 8; i++)
		ChildrenNodes[i].reset();

	PendingInsertion.clear();
	UnloadHelper(shared_from_this());
	TreeBuilt = false;
	TreeReady = false;
}

/// <summary>
/// Recursive helper function for removing all nodes in the tree
/// </summary>
/// <param name="root">The root node to start deleting from</param>
void OctTree::UnloadHelper(shared_ptr<OctTree> root)
{
	if (root == nullptr) 
		return;

	if (!root->NodeObjects.empty())
	{
		root->NodeObjects.clear();
		//root.m_objects = null;
	}

	if (Region != nullptr)
	{
		root->Region->Max = Vector3::Zero;
		root->Region->Min = Vector3::Zero;
	}

	if (root->ChildrenNodes != nullptr && root->ActiveNodes != 0)
	{
		for (int a = 0; a < 8; a++)
		{
			if (root->ChildrenNodes[a] != nullptr)
			{
				root->UnloadHelper(root->ChildrenNodes[a]);
				root->ChildrenNodes[a] = nullptr;
			}
		}
	}

	ActiveNodes = 0;
	//root.m_childNode = null;
	root->ParentNode = nullptr;
}

/// <summary>
/// Inserts a bunch of items into the oct tree.
/// </summary>
/// <param name="ItemList">A list of physical objects to add</param>
/// <remarks>The OctTree will be rebuilt JIT</remarks>
void OctTree::Enqueue(list<PhysicsComponentPtr> items)
{
	for each(PhysicsComponentPtr item in items)
	{
		PendingInsertion.push_back(item);
		TreeReady = false;
	}
}

void OctTree::Enqueue(PhysicsComponentPtr item)
{
	//are we trying to add at the root node? If so, we can assume that the user doesn't know where in the tree it needs to go.
	if (ParentNode == nullptr)
	{
		PendingInsertion.push_back(item);
		TreeReady = false;    //mark the tree as needing an update
	}
	else
	{
		//the user is giving us a hint on where in the tree they think the object should go. Let's try to insert as close to the hint as possible.
		shared_ptr<OctTree> current = shared_from_this();

		//push the object up the tree until we find a region which contains it
		ColliderType colliderType = item->ColliderBounding->Type;
		if (colliderType == ColliderType::AABB)
		{
			ColliderAABBptr objBoundingBox = dynamic_pointer_cast<ColliderAABB>(item->ColliderBounding);

			while (current->Region->Bounding.Contains(objBoundingBox->Bounding) != ContainmentType::CONTAINS)
				if (current->ParentNode != nullptr) 
					current = current->ParentNode;
				else 
					break; //prevent infinite loops when we go out of bounds of the root node region
		}
		else
			if (colliderType == ColliderType::Sphere)
			{
				ColliderSpherePtr objBoundingSphere = dynamic_pointer_cast<ColliderSphere>(item->ColliderBounding);

				while (current->Region->Bounding.Contains(objBoundingSphere->Bounding) != ContainmentType::CONTAINS)//we must be using a bounding sphere, so check for its containment.
					if (current->ParentNode != nullptr) 
						current = current->ParentNode;
					else break;
			}

		//push the object down the tree if we can.
		current->Insert(item);
	}
}

PhysicsComponentPtr OctTree::Dequeue()
{
	PhysicsComponentPtr element = PendingInsertion.front();
	PendingInsertion.pop_front();

	return element;
}

/// <summary>
/// Processes all pending insertions by inserting them into the tree.
/// </summary>
/// <remarks>Consider deprecating this?</remarks>
void OctTree::ProcessPendingItems()   //complete & tested
{
	if (ParentNode == nullptr)
		Root = shared_from_this();

	if (NodeObjects.empty())
		NodeObjects = list<PhysicsComponentPtr>();

	AllTreeObjects.clear();
	
	for each(PhysicsComponentPtr obj in PendingInsertion)
	{
		AllTreeObjects.push_back(obj);
	}


	/*I think I can just directly insert items into the tree instead of using a queue.*/
	if (!TreeBuilt)
	{
		for each(PhysicsComponentPtr obj in PendingInsertion)
		{
			AllTreeObjects.push_back(obj);
		}

		PendingInsertion.clear();

		//trim out any objects which have the exact same bounding areas

		BuildTree();

		TreeBuilt = true;
		TreeReady = true;     //we know that since no tree existed, this is the first time we're ever here.
	}
	else
	{
		//A tree structure exists already, so we just want to try to insert into the existing structure.
		//bug test: what if the pending item doesn't fit into the bounding region of the existing tree?
		while (PendingInsertion.size() != 0)
		{
			Insert(Dequeue());
		}
	}
}

void OctTree::UpdateTree() //complete & tested 
{
	if (!TreeBuilt)
	{
		while (PendingInsertion.size() != 0)
		{
			PhysicsComponentPtr element = Dequeue();
			NodeObjects.push_back(element);
			AllTreeObjects.push_back(element);
		}
			
		BuildTree();
	}
	else
	{
		while (PendingInsertion.size() != 0)
		{
			PhysicsComponentPtr element = Dequeue();
			Insert(element);
			AllTreeObjects.push_back(element);
		}
	}

	TreeReady = true;
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
	//auto test = NodeObjects.size();
	//terminate the recursion if we're a leaf node
	if (NodeObjects.size() <= 1)
		return;

	Vector3 dimensions = Region->Max - Region->Min;

	if (dimensions == Vector3::Zero)
	{
		//FindEnclosingCube();
		Region->CalculateBounding();
		dimensions = Region->Max - Region->Min;
	}

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
	auto testNodeObj = NodeObjects.size();
	for each(PhysicsComponentPtr obj in NodeObjects)
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
		else 
			if (colliderType == ColliderType::Sphere)
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
		auto testOctList1 = octList[0].size();
		auto testOctList2 = octList[1].size();
		auto testOctList3 = octList[2].size();
		auto testOctList4 = octList[3].size();
		auto testOctList5 = octList[4].size();
		auto testOctList6 = octList[5].size();
		auto testOctList7 = octList[6].size();
	}

	//delist every moved object from this node.
	for each(PhysicsComponentPtr obj in delist)
	{
		NodeObjects.remove(obj);
	}

	//Create child nodes where there are items contained in the bounding region
	for (int a = 0; a < 8; a++)
	{
		if (octList[a].size() != 0)
		{
			ChildrenNodes[a] = CreateNode(octant[a], octList[a]);
			ActiveNodes |= (byte)(1 << a);
			ChildrenNodes[a]->BuildTree();
		}
	}

	TreeBuilt = true;
	TreeReady = true;

}

shared_ptr<OctTree> OctTree::CreateNode(ColliderAABBptr region, list<PhysicsComponentPtr> objList) //complete & tested
{
	if (objList.size() == 0)
		return nullptr;

	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->ParentNode = shared_from_this();
	return ret;
}

shared_ptr<OctTree> OctTree::CreateNode(ColliderAABBptr region, PhysicsComponentPtr Item)
{
	list<PhysicsComponentPtr> objList(1); //sacrifice potential CPU time for a smaller memory footprint
	objList.push_back(Item);
	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->ParentNode = shared_from_this();
	return ret;
}

bool OctTree::HasChildren()
{
	bool hasChildren = false;

	for (int i = 0; i < 8; i++)
	{
		if (ChildrenNodes[i] != nullptr)
		{
			hasChildren = true;
			break;
		}
	}

	return hasChildren;
}

bool OctTree::IsRoot()
{
	if (ParentNode == nullptr)
		return true;
	else
		return false;
}

void OctTree::Update()
{
	DetectedCollisions.clear();

	if (TreeBuilt == true && TreeReady == true)
	{
		//Start a count down death timer for any leaf nodes which don't have objects or children.
		//when the timer reaches zero, we delete the leaf. If the node is reused before death, we double its lifespan.
		//this gives us a "frequency" usage score and lets us avoid allocating and deallocating memory unnecessarily
		if (NodeObjects.size() == 0)
		{
			if (HasChildren() == false)
			{
				if (CurLife == -1)
					CurLife = MaxLifespan;
				else if (CurLife > 0)
				{
					CurLife--;
				}
			}
		}
		else
		{
			if (CurLife != -1)
			{
				if (MaxLifespan <= 64)
					MaxLifespan *= 2;
				CurLife = -1;
			}
		}

		list<PhysicsComponentPtr> movedObjects(NodeObjects.size());
		auto testNodeObj = NodeObjects.size();

		//go through and update every object in the current tree node
		for each(PhysicsComponentPtr gameObj in NodeObjects)
		{
			//we should figure out if an object actually moved so that we know whether we need to update this node in the tree.
			if (gameObj->GetParent()->GetTransform()->GetUpdatedMoveFlag() == true)
			{
				movedObjects.push_back(gameObj);
			}
		}
		auto testMove = movedObjects.size();
		//prune any dead objects from the tree.
		for each(PhysicsComponentPtr object in NodeObjects)
		{
			if (object->CheckIfEnabled() == false)
			{
				list<PhysicsComponentPtr>::iterator it;
				it = std::find(movedObjects.begin(), movedObjects.end(), object);

				if (it != movedObjects.end())
					movedObjects.erase(it);

				NodeObjects.remove(object);
				AllTreeObjects.remove(object);
			}
		}
		
		//prune out any dead branches in the tree
		for (int flags = ActiveNodes, index = 0; flags > 0; flags >>= 1, index++)
			if ((flags & 1) == 1 && ChildrenNodes[index]->CurLife == 0)
			{
				if (ChildrenNodes[index]->NodeObjects.size() > 0)
				{
					//throw new Exception("Tried to delete a used branch!");
					ChildrenNodes[index]->CurLife = -1;
				}
				else
				{
					ChildrenNodes[index] = nullptr;
					ActiveNodes ^= (byte)(1 << index);       //remove the node from the active nodes flag list
				}
			}
		auto testFlag = ActiveNodes;
		//recursively update any child nodes.
		for (int flags = ActiveNodes, index = 0; flags > 0; flags >>= 1, index++)
		{
			auto test1 = true;
			if ((flags & 1) == 1)
			{
				if (ChildrenNodes != nullptr && ChildrenNodes[index] != nullptr)
					ChildrenNodes[index]->Update();
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
					if (current->ParentNode != nullptr) 
						current = current->ParentNode;
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
						if (current->ParentNode != nullptr)
						{
							current = current->ParentNode;
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
				NodeObjects.remove(movedObj);
				current->Insert(movedObj);   //this will try to insert the object as deep into the tree as we can go.

		}

		if (IsRoot())
		{
			//This will recursively gather up all collisions and create a list of them.
			//this is simply a matter of comparing all objects in the current root node with all objects in all child nodes.
			//note: we can assume that every collision will only be between objects which have moved.
			//note 2: An explosion can be centered on a point but grow in size over time. In this case, you'll have to override the update method for the explosion.
			list<CollisionPtr> irList = GetIntersection(list<PhysicsComponentPtr>());

			/*for each(CollisionPtr ir in irList)
			{
				if (ir->OriginObject != nullptr)
					ir->OriginObject.HandleIntersection(ir);
				if (ir->CollidingObject != nullptr)
					ir->CollidingObject.HandleIntersection(ir);
			}*/

			DetectedCollisions = irList;
		}
	}//end if tree built
	else
	{
		if (PendingInsertion.size() > 0)
		{
			ProcessPendingItems();
			Update();   //try this again...
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
		NodeObjects.push_back(Item);
		return true;
	}

	//Check to see if the dimensions of the box are greater than the minimum dimensions.
	//If we're at the smallest size, just insert the item here. We can't go any lower!
	Vector3 dimensions = Region->Max - Region->Min;
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		NodeObjects.push_back(Item);
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
		if (ParentNode != nullptr)
			return ParentNode->Insert(Item);
		else
			return false;
	}
	else
	{
		if (objBoundingSphere != nullptr && Region->Bounding.Contains(objBoundingSphere->Bounding) != ContainmentType::CONTAINS)
		{
			if (ParentNode != nullptr)
				return ParentNode->Insert(Item);
			else
				return false;
		}
	}

	//At this point, we at least know this region can contain the object but there are child nodes. Let's try to see if the object will fit
	//within a subregion of this region.

	Vector3 center = Region->Bounding.Center;

	//Find or create subdivided regions for each octant in the current region
	ColliderAABBptr childOctant[8];
	childOctant[0] = (ChildrenNodes[0] != nullptr) ? ChildrenNodes[0]->Region : make_shared<ColliderAABB>(Region->Min, center, true);
	childOctant[1] = (ChildrenNodes[1] != nullptr) ? ChildrenNodes[1]->Region : make_shared<ColliderAABB>(Vector3(center.x, Region->Min.y, Region->Min.z), Vector3(Region->Max.x, center.y, center.z));
	childOctant[2] = (ChildrenNodes[2] != nullptr) ? ChildrenNodes[2]->Region : make_shared<ColliderAABB>(Vector3(center.x, Region->Min.y, center.z), Vector3(Region->Max.x, center.y, Region->Max.z));
	childOctant[3] = (ChildrenNodes[3] != nullptr) ? ChildrenNodes[3]->Region : make_shared<ColliderAABB>(Vector3(Region->Min.x, Region->Min.y, center.z), Vector3(center.x, center.y, Region->Max.z));
	childOctant[4] = (ChildrenNodes[4] != nullptr) ? ChildrenNodes[4]->Region : make_shared<ColliderAABB>(Vector3(Region->Min.x, center.y, Region->Min.z), Vector3(center.x, Region->Max.y, center.z));
	childOctant[5] = (ChildrenNodes[5] != nullptr) ? ChildrenNodes[5]->Region : make_shared<ColliderAABB>(Vector3(center.x, center.y, Region->Min.z), Vector3(Region->Max.x, Region->Max.y, center.z));
	childOctant[6] = (ChildrenNodes[6] != nullptr) ? ChildrenNodes[6]->Region : make_shared<ColliderAABB>(center, Region->Max);
	childOctant[7] = (ChildrenNodes[7] != nullptr) ? ChildrenNodes[7]->Region : make_shared<ColliderAABB>(Vector3(Region->Min.x, center.y, center.z), Vector3(center.x, Region->Max.y, Region->Max.z));

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
				if (ChildrenNodes[a] != nullptr)
				{
					return ChildrenNodes[a]->Insert(Item);   //Add the item into that tree and let the child tree figure out what to do with it
				}
				else
				{
					ChildrenNodes[a] = CreateNode(childOctant[a], Item);   //create a new tree node with the item
					ActiveNodes |= (byte)(1 << a);
				}
				found = true;
			}
		}

		//we couldn't fit the item into a smaller box, so we'll have to insert it in this region
		if (!found)
		{
			NodeObjects.push_back(Item);
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
					if (ChildrenNodes[a] != nullptr)
					{
						return ChildrenNodes[a]->Insert(Item);   //Add the item into that tree and let the child tree figure out what to do with it
					}
					else
					{
						ChildrenNodes[a] = CreateNode(childOctant[a], Item);   //create a new tree node with the item
						ActiveNodes |= (byte)(1 << a);
					}
					found = true;
				}
			}

			//we couldn't fit the item into a smaller box, so we'll have to insert it in this region
			if (!found)
			{
				NodeObjects.push_back(Item);
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

	if (!TreeBuilt) 
		return list<CollisionPtr>();

	if (NodeObjects.size() == 0 && HasChildren() == false)   //terminator for any recursion
		return list<CollisionPtr>();

	list<CollisionPtr> ret = list<CollisionPtr>();

	//test each object in the list for intersection
	for each(PhysicsComponentPtr obj in NodeObjects)
	{
		//test for intersection
		CollisionPtr ir = Collision::CheckCollision(make_shared<PhysicsComponent>(colliderFrustum), obj);
		if (ir != nullptr)
			ret.push_back(ir);
	}

	//test each object in the list for intersection
	for (int a = 0; a < 8; a++)
	{
		if (ChildrenNodes[a] != nullptr && (frustum.Contains(ChildrenNodes[a]->Region->Bounding) == ContainmentType::INTERSECTS || frustum.Contains(ChildrenNodes[a]->Region->Bounding) == ContainmentType::CONTAINS))
		{
			list<CollisionPtr> hitList = ChildrenNodes[a]->GetIntersection(colliderFrustum);

			if (!hitList.empty())
			{
				copy(hitList.begin(), hitList.end(),
					back_insert_iterator<list<CollisionPtr> >(ret));
			}
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
	if (!TreeBuilt)
		return list<CollisionPtr>();

	if (NodeObjects.size() == 0 && HasChildren() == false)   //terminator for any recursion
		return list<CollisionPtr>();

	list<CollisionPtr> ret = list<CollisionPtr>();

	//the ray is intersecting this region, so we have to check for intersection with all of our contained objects and child regions.

	//test each object in the list for intersection
	for each(PhysicsComponentPtr obj in NodeObjects)
	{
		CollisionPtr ir = Collision::CheckCollision(obj, intersectRay);
		if (ir != nullptr)
			ret.push_back(ir);
	}

	// test each child octant for intersection
	for (int a = 0; a < 8; a++)
	{
		if (ChildrenNodes[a] != nullptr && Collision::CheckCollision(make_shared<PhysicsComponent>(ChildrenNodes[a]->Region), intersectRay) != nullptr)
		{
			list<CollisionPtr> hits = ChildrenNodes[a]->GetIntersection(intersectRay);

			if (!hits.empty())
			{
				copy(hits.begin(), hits.end(),
					back_insert_iterator<list<CollisionPtr> >(ret));
			}
		}
	}

	return ret;
}

list<CollisionPtr> OctTree::GetIntersection(list<PhysicsComponentPtr> parentObjs)
{
	list<CollisionPtr> intersections = list<CollisionPtr>();
	//assume all parent objects have already been processed for collisions against each other.
	//check all parent objects against all objects in our local node
	for each(PhysicsComponentPtr pObj in parentObjs)
	{
		for each(PhysicsComponentPtr lObj in NodeObjects)
		{
			//We let the two objects check for collision against each other. They can figure out how to do the coarse and granular checks.
			//all we're concerned about is whether or not a collision actually happened.
			CollisionPtr ir = Collision::CheckCollision(pObj, lObj);
			if (ir != nullptr)
			{
				//ir.m_treeNode = this;
				intersections.push_back(ir);
			}
		}
	}

	//now, check all our local objects against all other local objects in the node
	if (!NodeObjects.empty() && NodeObjects.size() > 1)
	{
		/*
			* This is a rather brilliant section of code. Normally, you'd just have two foreach loops, like so:
			* foreach(Physical lObj1 in m_objects)
			* {
			*      foreach(Physical lObj2 in m_objects)
			*      {
			*           //intersection check code
			*      }
			* }
			*
			* The problem is that this runs in O(N*N) time and that we're checking for collisions with objects which have already been checked.
			* Imagine you have a set of four items: {1,2,3,4}
			* You'd first check: {1} vs {1,2,3,4}
			* Next, you'd check {2} vs {1,2,3,4}
			* but we already checked {1} vs {2}, so it's a waste to check {2} vs. {1}. What if we could skip this check by removing {1}?
			* We'd have a total of 4+3+2+1 collision checks, which equates to O(N(N+1)/2) time. If N is 10, we are already doing half as many collision checks as necessary.
			* Now, we can't just remove an item at the end of the 2nd for loop since that would break the iterator in the first foreach loop, so we'd have to use a
			* regular for(int i=0;i<size;i++) style loop for the first loop and reduce size each iteration. This works...but look at the for loop: we're allocating memory for
			* two additional variables: i and size. What if we could figure out some way to eliminate those variables?
			* So, who says that we have to start from the front of a list? We can start from the back end and still get the same end results. With this in mind,
			* we can completely get rid of a for loop and use a while loop which has a conditional on the capacity of a temporary list being greater than 0.
			* since we can poll the list capacity for free, we can use the capacity as an indexer into the list items. Now we don't have to increment an indexer either!
			* The result is below.
			*/

		list<PhysicsComponentPtr> tmp = list<PhysicsComponentPtr>(NodeObjects.size());
		copy(NodeObjects.begin(), NodeObjects.end(),
			back_insert_iterator<list<PhysicsComponentPtr> >(tmp));
		
		while (tmp.size() > 0)
		{
			for each(PhysicsComponentPtr lObj2 in tmp)
			{
				list<PhysicsComponentPtr>::iterator i = tmp.end();
				--i;

				if (*i == lObj2)
					continue;

				CollisionPtr ir = Collision::CheckCollision(tmp.back(), lObj2);

				if (ir != nullptr)
				{
					//ir.m_treeNode = this;
					intersections.push_back(ir);
				}
			}

			//remove this object from the temp list so that we can run in O(N(N+1)/2) time instead of O(N*N)
			tmp.remove(tmp.back());
		}

	}

	//now, merge our local objects list with the parent objects list, then pass it down to all children.
	copy(NodeObjects.begin(), NodeObjects.end(),
		back_insert_iterator<list<PhysicsComponentPtr> >(parentObjs));

	//each child node will give us a list of intersection records, which we then merge with our own intersection records.
	for (int flags = ActiveNodes, index = 0; flags > 0; flags >>= 1, index++)
	{
		if ((flags & 1) == 1)
		{
			if (ChildrenNodes != nullptr && ChildrenNodes[index] != nullptr)
			{
				list<CollisionPtr> collisionList = ChildrenNodes[index]->GetIntersection(parentObjs);
				copy(collisionList.begin(), collisionList.end(),
					back_insert_iterator<list<CollisionPtr> >(intersections));
			}
		}
	}

	return intersections;
}

