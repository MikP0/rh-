#include "pch.h"
#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem()
{
}


PhysicsSystem::~PhysicsSystem()
{
}

CollisionPtr PhysicsSystem::Collide(PhysicsComponentPtr collider1, PhysicsComponentPtr collider2)
{
	collider1->ColliderBounding.CollisionKind = DISJOINT;
	collider2->ColliderBounding.CollisionKind = DISJOINT;

	ContainmentType collider1Result = collider1->ColliderBounding.Bounding.Contains(collider2->ColliderBounding.Bounding);
	ContainmentType collider2Result = collider2->ColliderBounding.Bounding.Contains(collider1->ColliderBounding.Bounding);

	collider1->ColliderBounding.CollisionKind = collider1Result;
	collider2->ColliderBounding.CollisionKind = collider2Result;

	if (collider1Result == INTERSECTS && collider2Result == INTERSECTS)
	{
		return (std::make_shared<Collision>(collider1->GetParent(), collider2->GetParent(), INTERSECTS));
	}

	if (collider1Result == CONTAINS)
	{
		return (std::make_shared<Collision>(collider1->GetParent(), collider2->GetParent(), CONTAINS));
	}

	if (collider2Result == CONTAINS)
	{
		return (std::make_shared<Collision>(collider2->GetParent(), collider1->GetParent(), CONTAINS));
	}


	return nullptr;
}

void PhysicsSystem::UpdateColliders()
{
	for each (PhysicsComponentPtr component in _components)
	{
		dxmath::Matrix objectMatrix = component->GetParent()->GetWorldMatrix();
		dxmath::Vector3 objectPosition = DirectX::XMVector3Transform(dxmath::Vector3::Zero, objectMatrix);
		component->ColliderBounding.Bounding.Center = objectPosition;
	}
}


std::vector<ComponentPtr> PhysicsSystem::GetComponents(ComponentType componentType)
{
	std::vector<ComponentPtr> allComponents;
	std::vector<ComponentPtr> selectedComponents;
	//allComponents = componentManager.GetAllComponents();

	for each (ComponentPtr component in allComponents)
	{
		if (std::dynamic_pointer_cast<PhysicsComponent<ColliderAABB>>(component)->GetType().name.compare(componentType.name) == 0)
		{
			selectedComponents.push_back(component);
		}
	}

	return selectedComponents;
}

void PhysicsSystem::UpdateComponentsCollection()
{
	_components.clear();
	std::vector<ComponentPtr> selectedComponents = GetComponents(_componentsType);
	for each (ComponentPtr component in selectedComponents)
	{
		PhysicsComponentPtr physicsComponent = std::dynamic_pointer_cast<PhysicsComponent<ColliderAABB>>(component);
		_components.push_back(physicsComponent);
	}
}

void PhysicsSystem::InsertComponent(PhysicsComponentPtr component)
{
	_components.push_back(component);
}

void PhysicsSystem::Iterate()
{
	for each (PhysicsComponentPtr component in _components)
	{

	}
}
