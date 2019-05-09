#include "pch.h"
#include "Collision.h"


Collision::Collision(EntityPtr origin, EntityPtr collider, ContainmentType collisionKind)
{
	OriginObject = origin;
	CollidingObject = collider;
	CollisionKind = collisionKind;
}

Collision::Collision(EntityPtr origin, ContainmentType collisionKind, float rayIntersectDist)
{
	OriginObject = origin;
	CollidingObject = nullptr;
	CollisionKind = collisionKind;
	RayIntersectDist = rayIntersectDist;
}

Collision::~Collision()
{
}

XMVECTORF32 Collision::GetCollisionColor(DirectX::ContainmentType collisionKind)
{
	if (collisionKind == DirectX::DISJOINT)
		return DirectX::Colors::White;
	else
		if (collisionKind == DirectX::INTERSECTS)
			return DirectX::Colors::Yellow;
		else
			if (collisionKind == DirectX::CONTAINS)
				return DirectX::Colors::Red;
	return DirectX::Colors::Green;
}

ContainmentTypeVector Collision::Collide(ColliderAABBptr collider1, ColliderAABBptr collider2)
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

ContainmentTypeVector Collision::Collide(ColliderSpherePtr collider1, ColliderSpherePtr collider2)
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

ContainmentTypeVector Collision::Collide(ColliderAABBptr collider1, ColliderSpherePtr collider2)
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

ContainmentTypeVector Collision::Collide(ColliderSpherePtr collider1, ColliderAABBptr collider2)
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

float Collision::Collide(ColliderAABBptr collider, ColliderRay ray)
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

float Collision::Collide(ColliderSpherePtr collider, ColliderRay ray)
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

shared_ptr<Collision> Collision::CheckCollision(PhysicsComponentPtr component1, PhysicsComponentPtr component2)
{
	ColliderType colliderType1 = component1->ColliderBounding->Type;
	ColliderType colliderType2 = component2->ColliderBounding->Type;

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
			return (std::make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
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
			return (std::make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
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
			return (std::make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
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
			return (std::make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
		}
	}

	return nullptr;
}

shared_ptr<Collision> Collision::CheckCollision(PhysicsComponentPtr component, ColliderRay ray)
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