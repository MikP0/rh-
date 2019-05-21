#include "pch.h"
#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem(Vector3 sceneCenter, float cubeDimension) : System()
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
			collider->Bounding.Center = objectPosition;
			continue;
		}

		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			collider->Bounding.Center = objectPosition;
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
		collider->Bounding.Center = objectPosition;
	}
	else
		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			collider->Bounding.Center = objectPosition;
		}
}


void PhysicsSystem::UpdateComponentsCollection()
{
	_components.clear();
	vector<PhysicsComponentPtr> selectedComponents = _world->GetComponents<PhysicsComponent>();
	for each (PhysicsComponentPtr component in selectedComponents)
	{
		_components.push_back(component);
	}
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
	/*shared_ptr<PhysicsComponent> colliderSceneWall = make_shared<PhysicsComponent>(AABB);
	shared_ptr<PhysicsComponent> colliderCup1 = make_shared<PhysicsComponent>(AABB);
	shared_ptr<PhysicsComponent> colliderCup2 = make_shared<PhysicsComponent>(Sphere);

	_entityManager->AddComponent(_entityManager->GetEntity("SceneWall"), colliderSceneWall);
	_entityManager->AddComponent(_entityManager->GetEntity("Cup1")->GetId(), colliderCup1);
	_entityManager->AddComponent(_entityManager->GetEntity("Cup2")->GetId(), colliderCup2);

	*/

	/*vector<shared_ptr<Component>> components = GetComponents(ComponentType("Physics"));

	for each (shared_ptr<Component> component in components)
	{
		auto test = component->GetParent()->GetName();
		_components.push_back(dynamic_pointer_cast<PhysicsComponent>(component));
	}*/

	_octTree->Region->Bounding = BoundingBox(_sceneCenter, Vector3(_sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f, _sceneCubeDimension / 2.0f));
	_octTree->Region->CalculateBounding();

	
	for each(PhysicsComponentPtr component in _components)
	{
		_octTree->PendingInsertion.push_back(component);
	}
	/*auto test1 = _octTree->PendingInsertion.size();
	auto test2 = _octTree->AllTreeObjects.size();*/
	//_octTree->UpdateTree();
	/*auto test3 = _octTree->PendingInsertion.size();
	auto test4 = _octTree->AllTreeObjects.size();*/
}

void PhysicsSystem::Iterate()
{
	for each (PhysicsComponentPtr component in _components)
	{
		UpdateCollidersPositions();
		/*auto test1 = _octTree->PendingInsertion.size();
		auto test2 = _octTree->AllTreeObjects.size();*/
		//_octTree->UpdateTree();
		/*auto test3 = _octTree->PendingInsertion.size();
		auto test4 = _octTree->AllTreeObjects.size();*/
		_octTree->Update();
		ResetAllUpdatePositionFlags();
	}
}

