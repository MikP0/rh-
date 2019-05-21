#include "pch.h"
#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(shared_ptr<EntityManager> entityManager, Vector3 sceneCenter, float cubeDimension) : System(entityManager)
{
	_componentsType._name = "Physics";
	_sceneCenter = sceneCenter;
	_sceneCubeDimension = cubeDimension;
	ColliderAABBptr region = make_shared<ColliderAABB>();
	_octTree = make_shared<OctTree>(region);
}


PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::UpdateCollidersPositions()
{
	for each (PhysicsComponentPtr component in _components)
	{
		dxmath::Matrix objectMatrix = component->GetParent()->GetWorldMatrix();
		dxmath::Vector3 objectPosition = DirectX::XMVector3Transform(dxmath::Vector3::Zero, objectMatrix);;

		if (component->ColliderBounding->Type == AABB)
		{
			ColliderAABBptr collider = dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding);
			collider->SetCenter(objectPosition);
			continue;
		}

		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			collider->SetCenter(objectPosition);
			continue;
		}	
	}
}

void PhysicsSystem::UpdateColliderPosition(PhysicsComponentPtr component)
{
	dxmath::Matrix objectMatrix = component->GetParent()->GetWorldMatrix();
	dxmath::Vector3 objectPosition = DirectX::XMVector3Transform(dxmath::Vector3::Zero, objectMatrix);;

	if (component->ColliderBounding->Type == AABB)
	{
		ColliderAABBptr collider = dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding);
		collider->SetCenter(objectPosition);
	}
	else
		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			collider->SetCenter(objectPosition);
		}
}


vector<ComponentPtr> PhysicsSystem::GetComponents(ComponentType componentType)
{
	vector<ComponentPtr> result = _entityManager->GetComponents(_componentsType);

	return result;
}

void PhysicsSystem::UpdateComponentsCollection()
{
	_components.clear();
	vector<ComponentPtr> selectedComponents = GetComponents(_componentsType);
	for each (ComponentPtr component in selectedComponents)
	{
		PhysicsComponentPtr physicsComponent = dynamic_pointer_cast<PhysicsComponent>(component);
		_components.push_back(physicsComponent);
	}
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
	for each(PhysicsComponentPtr component in _components)
	{
		if (component->GetParent()->GetTransform()->GetUpdatedMoveFlag())
			return true;
	}

	return false;
}

void PhysicsSystem::ResetAllUpdatePositionFlags()
{
	for each(PhysicsComponentPtr component in _components)
	{
		component->GetParent()->GetTransform()->SetUpdatedMoveFlag(false);
	}
}

void PhysicsSystem::InsertComponent(PhysicsComponentPtr component)
{
	_components.push_back(component);
}

void PhysicsSystem::Initialize()
{
	_octTree->Region->SetBounding(BoundingBox(_sceneCenter, Vector3(_sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f)));
	_octTree->Root = _octTree;

	UpdateCollidersPositions();
	_octTree->UnloadContent();
	_octTree->InsertIntoTree(_components);
	_octTree->BuildTree();
}

void PhysicsSystem::Iterate()
{
	UpdateCollidersPositions();

	if (CheckIfShouldUpdateTree())
	{
		vector<PhysicsComponentPtr> activeComponents = vector<PhysicsComponentPtr>();

		for each(PhysicsComponentPtr component in _components)
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

