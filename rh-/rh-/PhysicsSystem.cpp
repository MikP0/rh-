#include "pch.h"
#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem(std::shared_ptr<EntityManager> entityManager) : System(entityManager)
{
	_componentsType._name = "Physics";
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
			ColliderAABBptr collider = std::dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding);
			collider->Bounding.Center = objectPosition;
			continue;
		}

		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = std::dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
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
		ColliderAABBptr collider = std::dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding);
		collider->Bounding.Center = objectPosition;
	}
	else
		if (component->ColliderBounding->Type == Sphere)
		{
			ColliderSpherePtr collider = std::dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding);
			collider->Bounding.Center = objectPosition;
		}
}


std::vector<ComponentPtr> PhysicsSystem::GetComponents(ComponentType componentType)
{
	vector<ComponentPtr> result = _entityManager->GetComponents(_componentsType);

	return result;
}

void PhysicsSystem::UpdateComponentsCollection()
{
	_components.clear();
	std::vector<ComponentPtr> selectedComponents = GetComponents(_componentsType);
	for each (ComponentPtr component in selectedComponents)
	{
		PhysicsComponentPtr physicsComponent = std::dynamic_pointer_cast<PhysicsComponent>(component);
		_components.push_back(physicsComponent);
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
	shared_ptr<PhysicsComponent> colliderCup1 = std::make_shared<PhysicsComponent>(AABB);
	shared_ptr<PhysicsComponent> colliderCup2 = std::make_shared<PhysicsComponent>(Sphere);

	_entityManager->AddComponent(_entityManager->GetEntity("SceneWall"), colliderSceneWall);
	_entityManager->AddComponent(_entityManager->GetEntity("Cup1")->GetId(), colliderCup1);
	_entityManager->AddComponent(_entityManager->GetEntity("Cup2")->GetId(), colliderCup2);

	*/

	vector<shared_ptr<Component>> components = GetComponents(ComponentType("Physics"));

	for each (shared_ptr<Component> component in components)
	{
		auto test = component->GetParent()->GetName();
		_components.push_back(dynamic_pointer_cast<PhysicsComponent>(component));
	}
}

void PhysicsSystem::Iterate()
{
	for each (PhysicsComponentPtr component in _components)
	{

		ResetAllUpdatePositionFlags();
	}
}

