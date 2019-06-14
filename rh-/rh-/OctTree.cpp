/*Note: we want to avoid allocating memory for as long as possible since 
there can be lots of nodes.*/
/// 
/// Creates an oct tree which encloses the given region and contains the provided objects. 
///

#include "pch.h"
#include "OctTree.h"

shared_ptr<OctTree> OctTree::Root = nullptr;
list<PhysicsComponentPtr> OctTree::AllTreeObjects = list<PhysicsComponentPtr>();

OctTree::OctTree(ColliderAABBptr region)
{
	Region = region;
	NodeObjects = list<PhysicsComponentPtr>();
	ParentNode = nullptr;
	ChildrenNodes = vector<shared_ptr<OctTree>>(8, nullptr);
	ActiveNodes = 0;
}

OctTree::OctTree(ColliderAABBptr region, list<PhysicsComponentPtr> objList)
{
	Region = region;
	NodeObjects = objList;
	ParentNode = nullptr;
	ChildrenNodes = vector<shared_ptr<OctTree>>(8, nullptr);
	ActiveNodes = 0;
}

OctTree::~OctTree()
{
}

bool OctTree::HasChildren()
{
	if (ActiveNodes != 0)
		return true;
	else
		return false;
}

bool OctTree::IsRoot()
{
	if (ParentNode == nullptr)
		return true;
	else
		return false;
}

void OctTree::UnloadContent()
{
	UnloadHelper(shared_from_this());

	if (IsRoot())
	{
		AllTreeObjects.clear();
		ChildrenNodes = vector<shared_ptr<OctTree>>(8, nullptr);
	}
}

void OctTree::UnloadHelper(shared_ptr<OctTree> root)
{
	if (root == nullptr)
		return;

	if (!root->NodeObjects.empty())
	{
		root->NodeObjects.clear();
	}

	if (!IsRoot() && Region != nullptr)
	{
		root->Region = nullptr;
	}

	if (!root->ChildrenNodes.empty() && root->ActiveNodes != 0)
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
	root->ParentNode = nullptr;
}

void OctTree::InsertIntoTree(vector<PhysicsComponentPtr> objects)
{
	AllTreeObjects = list<PhysicsComponentPtr>(objects.begin(), objects.end());
	NodeObjects = AllTreeObjects;
	Root = shared_from_this();
}

void OctTree::BuildTree()
{
	if (NodeObjects.size() <= 1)
		return;

	Vector3 dimensions = Region->GetMax() - Region->GetMin();

	if (dimensions == Vector3::Zero)
	{
		//FindEnclosingCube();
		Region->CalculateDimBounding();
		dimensions = Region->GetMax() - Region->GetMin();
	}

	//Check to see if the dimensions of the box are greater than the minimum dimensions
	if (dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
	{
		return;
	}

	Vector3 center = Region->GetCenter();

	//Create subdivided regions for each octant
	vector<ColliderAABBptr> octant(8, nullptr);

	octant[0] = make_shared<ColliderAABB>(Region->GetMin(), center, true);
	octant[1] = make_shared<ColliderAABB>(Vector3(center.x, Region->GetMin().y, Region->GetMin().z), Vector3(Region->GetMax().x, center.y, center.z), true);
	octant[2] = make_shared<ColliderAABB>(Vector3(center.x, Region->GetMin().y, center.z), Vector3(Region->GetMax().x, center.y, Region->GetMax().z), true);
	octant[3] = make_shared<ColliderAABB>(Vector3(Region->GetMin().x, Region->GetMin().y, center.z), Vector3(center.x, center.y, Region->GetMax().z), true);
	octant[4] = make_shared<ColliderAABB>(Vector3(Region->GetMin().x, center.y, Region->GetMin().z), Vector3(center.x, Region->GetMax().y, center.z), true);
	octant[5] = make_shared<ColliderAABB>(Vector3(center.x, center.y, Region->GetMin().z), Vector3(Region->GetMax().x, Region->GetMax().y, center.z), true);
	octant[6] = make_shared<ColliderAABB>(center, Region->GetMax(), true);
	octant[7] = make_shared<ColliderAABB>(Vector3(Region->GetMin().x, center.y, center.z), Vector3(center.x, Region->GetMax().y, Region->GetMax().z), true);

	//This will contain all of our objects which fit within each respective octant.
	vector<list<PhysicsComponentPtr>> octList = vector<list<PhysicsComponentPtr>>(8, list<PhysicsComponentPtr>());

	//this list contains all of the objects which got moved down the tree and can be delisted from this node.
	list<PhysicsComponentPtr> delist = list<PhysicsComponentPtr>();

	for each(PhysicsComponentPtr obj in NodeObjects)
	{
		ColliderType colliderType = obj->ColliderBounding->Type;

		//if (colliderType == ColliderType::AABB)
		//{
			ColliderAABBptr objBoundingBox = dynamic_pointer_cast<ColliderAABB>(obj->ColliderBounding);

			for (int a = 0; a < 8; a++)
			{
				ContainmentType ct = octant[a]->GetBounding().Contains(objBoundingBox->GetBounding());

				if (ct == ContainmentType::CONTAINS)
				{
					octList[a].push_back(obj);
					delist.push_back(obj);
					break;
				}
			}
		//}
		/*else
			if (colliderType == ColliderType::Sphere)
			{
				ColliderSpherePtr objBoundingSphere = dynamic_pointer_cast<ColliderSphere>(obj->ColliderBounding);

				for (int a = 0; a < 8; a++)
				{
					ContainmentType ct = octant[a]->GetBounding().Contains(objBoundingSphere->GetBounding());

					if (ct == ContainmentType::CONTAINS)
					{
						octList[a].push_back(obj);
						delist.push_back(obj);
						break;
					}
				}
			}*/
	}

	//delist every moved object from this node.
	for each(PhysicsComponentPtr obj in delist)
	{
		NodeObjects.remove(obj);
	}

	//Create child nodes where there are items contained in the bounding region
	for (int a = 0; a < 8; a++)
	{
		if (!octList[a].empty())
		{
			ChildrenNodes[a] = CreateNode(octant[a], octList[a]);
			ActiveNodes |= (byte)(1 << a);
			ChildrenNodes[a]->BuildTree();
		}
	}

}

shared_ptr<OctTree> OctTree::CreateNode(ColliderAABBptr region, list<PhysicsComponentPtr> objList) //complete & tested
{
	if (objList.empty())
		return nullptr;

	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->ParentNode = shared_from_this();

	return ret;
}

shared_ptr<OctTree> OctTree::CreateNode(ColliderAABBptr region, PhysicsComponentPtr item)
{
	if (item == nullptr)
		return nullptr;

	list<PhysicsComponentPtr> objList = list<PhysicsComponentPtr>();
	objList.push_back(item);
	shared_ptr<OctTree> ret = make_shared<OctTree>(region, objList);
	ret->ParentNode = shared_from_this();

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

			if (pObj->IsTriggered || lObj->IsTriggered)
				break;

			CollisionPtr ir = Collision::CheckCollision(pObj, lObj);

			if (ir != nullptr)
			{
				intersections.push_back(ir);
			}
		}
	}

	//now, check all our local objects against all other local objects in the node
	if (!NodeObjects.empty())
	{
		list<PhysicsComponentPtr> tmp = NodeObjects;

		bool isThereTriggered = false;

		while (!tmp.empty() && !isThereTriggered)
		{
			PhysicsComponentPtr lastElement = tmp.back();

			for each(PhysicsComponentPtr lObj2 in tmp)
			{
				if (lObj2->IsTriggered || lastElement->IsTriggered)
				{
					isThereTriggered = true;
					break;
				}

				if (lastElement == lObj2)
					continue;

				CollisionPtr ir = Collision::CheckCollision(lObj2, lastElement);

				if (ir != nullptr)
				{
					intersections.push_back(ir);
				}
			}

			//remove this object from the temp list so that we can run in O(N(N+1)/2) time instead of O(N*N)
			tmp.remove(lastElement);
		}

	}

	//now, merge our local objects list with the parent objects list, then pass it down to all children.
	copy(NodeObjects.begin(), NodeObjects.end(), back_insert_iterator<list<PhysicsComponentPtr> >(parentObjs));

	//each child node will give us a list of intersection records, which we then merge with our own intersection records.
	for (int flags = ActiveNodes, index = 0; flags > 0; flags >>= 1, index++)
	{
		if ((flags & 1) == 1)
		{
			if (!ChildrenNodes.empty() && ChildrenNodes[index] != nullptr)
			{
				list<CollisionPtr> collisionList = ChildrenNodes[index]->GetIntersection(parentObjs);
				copy(collisionList.begin(), collisionList.end(), back_insert_iterator<list<CollisionPtr> >(intersections));
			}
		}
	}

	return intersections;
}

list<CollisionPtr> OctTree::GetIntersection(ColliderRayPtr intersectRay)
{
	list<CollisionPtr> ret = list<CollisionPtr>();

	if (NodeObjects.empty() && HasChildren() == false)   //terminator for any recursion
		return ret;

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
		if (ChildrenNodes[a] != nullptr)
		{
			CollisionPtr coll = Collision::CheckCollision(make_shared<PhysicsComponent>(ChildrenNodes[a]->Region), intersectRay);

			if (coll != nullptr)
			{
				list<CollisionPtr> hits = ChildrenNodes[a]->GetIntersection(intersectRay);

				if (!hits.empty())
				{
					copy(hits.begin(), hits.end(), back_insert_iterator<list<CollisionPtr> >(ret));
				}
			}	
		}
	}

	return ret;
}

list<CollisionPtr> OctTree::GetIntersection(ColliderFrustumPtr colliderFrustum)
{
	list<CollisionPtr> ret = list<CollisionPtr>();

	if (NodeObjects.empty() && HasChildren() == false)   //terminator for any recursion
		return ret;

	//test each object in the list for intersection
	for each(PhysicsComponentPtr obj in NodeObjects)
	{
		CollisionPtr ir = Collision::CheckCollision(make_shared<PhysicsComponent>(colliderFrustum), obj);

		if (ir != nullptr)
			ret.push_back(ir);
	}

	//test each object in the list for intersection
	for (int a = 0; a < 8; a++)
	{
		if (ChildrenNodes[a] != nullptr)
		{
			CollisionPtr coll = Collision::CheckCollision(make_shared<PhysicsComponent>(colliderFrustum), make_shared<PhysicsComponent>(ChildrenNodes[a]->Region));

			if (coll != nullptr)
			{
				list<CollisionPtr> hits = ChildrenNodes[a]->GetIntersection(colliderFrustum);

				if (!hits.empty())
				{
					copy(hits.begin(), hits.end(), back_insert_iterator<list<CollisionPtr>>(ret));
				}
			}
		}
	}

	return ret;
}



