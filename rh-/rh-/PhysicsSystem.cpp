#include "pch.h"
#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(Vector3 sceneCenter, float cubeDimension) : System()
{
	_sceneCenter = sceneCenter;
	_sceneCubeDimension = cubeDimension;
	ColliderAABBptr region = make_shared<ColliderAABB>(XMFLOAT3(_sceneCenter), XMFLOAT3(Vector3(_sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f)));
	_octTree = make_shared<OctTree>(region);
}


PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::UpdateCollidersPositions()
{
	for (auto component : _world->GetComponents<PhysicsComponent>())
	{
		dxmath::Matrix objectMatrix = component->GetParent()->GetWorldMatrix();

		if (component->ColliderBounding->Type == AABB)
		{
			ColliderAABBptr collider = dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding);
			dxmath::Vector3 objectPosition =
				DirectX::XMVector3Transform(collider->GetPositionOffset(), objectMatrix);
			collider->SetCenter(objectPosition);
			continue;
		}

		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			dxmath::Vector3 objectPosition =
				DirectX::XMVector3Transform(collider->GetPositionOffset(), objectMatrix);
			collider->SetCenter(objectPosition);
			continue;
		}
	}
}

void PhysicsSystem::UpdateColliderPosition(PhysicsComponentPtr component)
{
	dxmath::Matrix objectMatrix = component->GetParent()->GetWorldMatrix();

	if (component->ColliderBounding->Type == AABB)
	{
		ColliderAABBptr collider = dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding);
		dxmath::Vector3 objectPosition =
			DirectX::XMVector3Transform(collider->GetPositionOffset(), objectMatrix);
		collider->SetCenter(objectPosition);
	}
	else
		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			dxmath::Vector3 objectPosition =
				DirectX::XMVector3Transform(collider->GetPositionOffset(), objectMatrix);
			collider->SetCenter(objectPosition);
		}
}

OctTreePtr PhysicsSystem::GetOctTree()
{
	return _octTree;
}

vector<CollisionPtr> PhysicsSystem::GetCurrentCollisions()
{
	list<CollisionPtr> currentCollisionList = _octTree->GetIntersection(list<PhysicsComponentPtr>());
	return vector<CollisionPtr>(currentCollisionList.begin(), currentCollisionList.end());
}

vector<CollisionPtr> PhysicsSystem::GetCollisionsWithRay(ColliderRayPtr ray)
{
	list<CollisionPtr> collisionList = _octTree->GetIntersection(ray);
	return vector<CollisionPtr>(collisionList.begin(), collisionList.end());
}

vector<CollisionPtr> PhysicsSystem::GetCollisionsWithFrustum(ColliderFrustumPtr frustum)
{
	list<CollisionPtr> collisionList = _octTree->GetIntersection(frustum);
	return vector<CollisionPtr>(collisionList.begin(), collisionList.end());
}

bool PhysicsSystem::CheckIfShouldUpdateTree()
{
	for (auto component : _world->GetComponents<PhysicsComponent>())
	{
		if (component->GetParent()->GetTransform()->GetUpdatedMoveFlag())
			return true;
	}

	return false;
}

void PhysicsSystem::ResetAllUpdatePositionFlags()
{
	for (auto component : _world->GetComponents<PhysicsComponent>())
	{
		component->GetParent()->GetTransform()->SetUpdatedMoveFlag(false);
	}
}

void PhysicsSystem::Initialize()
{
	//_octTree->Region->SetBounding(BoundingBox(_sceneCenter, Vector3(_sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f)));
	_octTree->Root = _octTree;

	UpdateCollidersPositions();
	_octTree->UnloadContent();
	_octTree->InsertIntoTree(_world->GetComponents<PhysicsComponent>());
	_octTree->BuildTree();
}

void PhysicsSystem::Iterate()
{
	UpdateCollidersPositions();

	if (CheckIfShouldUpdateTree())
	{
		vector<PhysicsComponentPtr> activeComponents = vector<PhysicsComponentPtr>();

		for (auto component : _world->GetComponents<PhysicsComponent>())
		{
			if (component->CheckIfEnabled())
				activeComponents.push_back(component);
		}

		_octTree->UnloadContent();
		_octTree->InsertIntoTree(activeComponents);
		_octTree->BuildTree();
		ResetAllUpdatePositionFlags();
	}

	/*vector<CollisionPtr> testCollisions = GetCurrentCollisions();
	if (testCollisions.size() > 0)
	{
		bool presentCollisions = true;
	}*/
}

vector<ColliderBasePtr> PhysicsSystem::GetColliders()
{
	vector<ColliderBasePtr> result;
	for (auto component : _world->GetComponents<PhysicsComponent>())
	{
		//dxmath::Matrix objectMatrix = component->GetParent()->GetWorldMatrix();

		if (component->ColliderBounding->Type == AABB)
		{
			ColliderAABBptr collider = dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding);
			result.push_back(collider);
			continue;
		}

		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			//dxmath::Vector3 objectPosition =
			//	DirectX::XMVector3Transform(collider->GetPositionOffset(), objectMatrix);
			//collider->SetCenter(objectPosition);
			result.push_back(collider);
			continue;
		}
	}
	return result;
}

