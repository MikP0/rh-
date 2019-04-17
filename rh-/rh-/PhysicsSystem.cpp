#include "pch.h"
#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem()
{
}


PhysicsSystem::~PhysicsSystem()
{
}

ContainmentTypeVector PhysicsSystem::Collide(ColliderAABBptr collider1, ColliderAABBptr collider2)
{
	collider1->CollisionKind = DISJOINT;
	collider2->CollisionKind = DISJOINT;

	ContainmentType collider1Result = collider1->Bounding.Contains(collider2->Bounding);
	ContainmentType collider2Result = collider2->Bounding.Contains(collider1->Bounding);

	collider1->CollisionKind = collider1Result;
	collider2->CollisionKind = collider2Result;

	ContainmentTypeVector result { collider1->CollisionKind, collider2->CollisionKind };

	return result;
}

ContainmentTypeVector PhysicsSystem::Collide(ColliderSpherePtr collider1, ColliderSpherePtr collider2)
{
	collider1->CollisionKind = DISJOINT;
	collider2->CollisionKind = DISJOINT;

	ContainmentType collider1Result = collider1->Bounding.Contains(collider2->Bounding);
	ContainmentType collider2Result = collider2->Bounding.Contains(collider1->Bounding);

	collider1->CollisionKind = collider1Result;
	collider2->CollisionKind = collider2Result;

	ContainmentTypeVector result { collider1->CollisionKind, collider2->CollisionKind };

	return result;
}

ContainmentTypeVector PhysicsSystem::Collide(ColliderAABBptr collider1, ColliderSpherePtr collider2)
{
	collider1->CollisionKind = DISJOINT;
	collider2->CollisionKind = DISJOINT;

	ContainmentType collider1Result = collider1->Bounding.Contains(collider2->Bounding);
	ContainmentType collider2Result = collider2->Bounding.Contains(collider1->Bounding);

	collider1->CollisionKind = collider1Result;
	collider2->CollisionKind = collider2Result;

	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };

	return result;
}

ContainmentTypeVector PhysicsSystem::Collide(ColliderSpherePtr collider1, ColliderAABBptr collider2)
{
	collider1->CollisionKind = DISJOINT;
	collider2->CollisionKind = DISJOINT;

	ContainmentType collider1Result = collider1->Bounding.Contains(collider2->Bounding);
	ContainmentType collider2Result = collider2->Bounding.Contains(collider1->Bounding);

	collider1->CollisionKind = collider1Result;
	collider2->CollisionKind = collider2Result;

	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };

	return result;
}

float PhysicsSystem::Collide(ColliderAABBptr collider, ColliderRay ray)
{
	float distance = 0.0f;

	float dist;
	if (collider->Bounding.Intersects(ray.Origin, ray.Direction, dist))
	{
		distance = dist;
		collider->CollisionKind = INTERSECTS;
	}
	else
		collider->CollisionKind = DISJOINT;

	return distance;
}

float PhysicsSystem::Collide(ColliderSpherePtr collider, ColliderRay ray)
{
	float distance = 0.0f;

	float dist;
	if (collider->Bounding.Intersects(ray.Origin, ray.Direction, dist))
	{
		distance = dist;
		collider->CollisionKind = INTERSECTS;
	}
	else
		collider->CollisionKind = DISJOINT;

	return distance;
}

CollisionPtr PhysicsSystem::CheckCollision(PhysicsComponentPtr component1, PhysicsComponentPtr component2)
{
	ColliderType colliderType1 = component1->ColliderBounding->Type;
	ColliderType colliderType2 = component1->ColliderBounding->Type;

	if (colliderType1 == AABB && colliderType2 == AABB)
	{
		ContainmentTypeVector collisionResult = Collide(std::dynamic_pointer_cast<ColliderAABB>(component1->ColliderBounding), std::dynamic_pointer_cast<ColliderAABB>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}
	}

	if (colliderType1 == Sphere && colliderType2 == Sphere)
	{
		ContainmentTypeVector collisionResult = Collide(std::dynamic_pointer_cast<ColliderSphere>(component1->ColliderBounding), std::dynamic_pointer_cast<ColliderSphere>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}
	}

	if (colliderType1 == AABB && colliderType2 == Sphere)
	{
		ContainmentTypeVector collisionResult = Collide(std::dynamic_pointer_cast<ColliderAABB>(component1->ColliderBounding), std::dynamic_pointer_cast<ColliderSphere>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}
	}

	if (colliderType1 == Sphere && colliderType2 == AABB)
	{
		ContainmentTypeVector collisionResult = Collide(std::dynamic_pointer_cast<ColliderSphere>(component1->ColliderBounding), std::dynamic_pointer_cast<ColliderAABB>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (std::make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}
	}

	return nullptr;
}

CollisionPtr PhysicsSystem::CheckCollision(PhysicsComponentPtr component, ColliderRay ray)
{
	ColliderType colliderType = component->ColliderBounding->Type;

	float resultDistance;

	if (colliderType == AABB)
	{
		resultDistance = Collide(std::dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding), ray);
		return std::make_shared<Collision>(component->GetParent(), component->ColliderBounding->CollisionKind, resultDistance);
	}
	else
		if (colliderType == Sphere)
		{
			resultDistance = Collide(std::dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding), ray);
			return std::make_shared<Collision>(component->GetParent(), component->ColliderBounding->CollisionKind, resultDistance);
		}

	return nullptr;
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
	std::vector<ComponentPtr> allComponents;
	std::vector<ComponentPtr> selectedComponents;
	//allComponents = componentManager.GetAllComponents();

	for each (ComponentPtr component in allComponents)
	{
		if (std::dynamic_pointer_cast<PhysicsComponent>(component)->GetType().name.compare(componentType.name) == 0)
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
		PhysicsComponentPtr physicsComponent = std::dynamic_pointer_cast<PhysicsComponent>(component);
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
