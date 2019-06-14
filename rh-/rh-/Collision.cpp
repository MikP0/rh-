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

Collision::Collision(shared_ptr<Collision> collision)
{
	OriginObject = collision->OriginObject;
	CollidingObject = collision->CollidingObject;
	CollisionKind = collision->CollisionKind;
	RayIntersectDist = collision->RayIntersectDist;
}

Collision::~Collision()
{
}

XMVECTORF32 Collision::GetCollisionColor(DirectX::ContainmentType collisionKind)
{
	if (collisionKind == DirectX::DISJOINT)
		return DirectX::Colors::GreenYellow;
	else
		if (collisionKind == DirectX::INTERSECTS)
			return DirectX::Colors::White;
		else
			if (collisionKind == DirectX::CONTAINS)
				return DirectX::Colors::Red;
	return DirectX::Colors::Green;
}

ContainmentTypeVector Collision::Collide(ColliderAABBptr collider1, ColliderAABBptr collider2)
{
	collider1->CollisionKind = DISJOINT;
	collider2->CollisionKind = DISJOINT;

	ContainmentType collider1Result = collider1->GetBounding().Contains(collider2->GetBounding());
	ContainmentType collider2Result = collider2->GetBounding().Contains(collider1->GetBounding());

	collider1->CollisionKind = collider1Result;
	collider2->CollisionKind = collider2Result;

	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };

	return result;
}

//ContainmentTypeVector Collision::Collide(ColliderSpherePtr collider1, ColliderSpherePtr collider2)
//{
//	collider1->CollisionKind = DISJOINT;
//	collider2->CollisionKind = DISJOINT;
//
//	ContainmentType collider1Result = collider1->GetBounding().Contains(collider2->GetBounding());
//	ContainmentType collider2Result = collider2->GetBounding().Contains(collider1->GetBounding());
//
//	collider1->CollisionKind = collider1Result;
//	collider2->CollisionKind = collider2Result;
//
//	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };
//
//	return result;
//}
//
//ContainmentTypeVector Collision::Collide(ColliderAABBptr collider1, ColliderSpherePtr collider2)
//{
//	collider1->CollisionKind = DISJOINT;
//	collider2->CollisionKind = DISJOINT;
//
//	ContainmentType collider1Result = collider1->GetBounding().Contains(collider2->GetBounding());
//	ContainmentType collider2Result = collider2->GetBounding().Contains(collider1->GetBounding());
//
//	collider1->CollisionKind = collider1Result;
//	collider2->CollisionKind = collider2Result;
//
//	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };
//
//	return result;
//}
//
//ContainmentTypeVector Collision::Collide(ColliderSpherePtr collider1, ColliderAABBptr collider2)
//{
//	collider1->CollisionKind = DISJOINT;
//	collider2->CollisionKind = DISJOINT;
//
//	ContainmentType collider1Result = collider1->GetBounding().Contains(collider2->GetBounding());
//	ContainmentType collider2Result = collider2->GetBounding().Contains(collider1->GetBounding());
//
//	collider1->CollisionKind = collider1Result;
//	collider2->CollisionKind = collider2Result;
//
//	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };
//
//	return result;
//}

ContainmentTypeVector Collision::Collide(ColliderFrustumPtr collider1, ColliderAABBptr collider2)
{
	collider1->CollisionKind = DISJOINT;
	collider2->CollisionKind = DISJOINT;

	ContainmentType collider1Result = collider1->GetBounding().Contains(collider2->GetBounding());
	ContainmentType collider2Result = collider2->GetBounding().Contains(collider1->GetBounding());

	collider1->CollisionKind = collider1Result;
	collider2->CollisionKind = collider2Result;

	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };

	return result;
}

//ContainmentTypeVector Collision::Collide(ColliderFrustumPtr collider1, ColliderSpherePtr collider2)
//{
//	collider1->CollisionKind = DISJOINT;
//	collider2->CollisionKind = DISJOINT;
//
//	ContainmentType collider1Result = collider1->GetBounding().Contains(collider2->GetBounding());
//	ContainmentType collider2Result = collider2->GetBounding().Contains(collider1->GetBounding());
//
//	collider1->CollisionKind = collider1Result;
//	collider2->CollisionKind = collider2Result;
//
//	ContainmentTypeVector result{ collider1->CollisionKind, collider2->CollisionKind };
//
//	return result;
//}

float Collision::Collide(ColliderAABBptr collider, ColliderRayPtr ray)
{
	float distance = 0.0f;

	float dist;
	if (collider->GetBounding().Intersects(ray->Origin, ray->Direction, dist))
	{
		distance = dist;
		collider->CollisionKind = INTERSECTS;
	}
	else
		collider->CollisionKind = DISJOINT;

	return distance;
}

//float Collision::Collide(ColliderSpherePtr collider, ColliderRayPtr ray)
//{
//	float distance = 0.0f;
//
//	float dist;
//	if (collider->GetBounding().Intersects(ray->Origin, ray->Direction, dist))
//	{
//		distance = dist;
//		collider->CollisionKind = INTERSECTS;
//	}
//	else
//		collider->CollisionKind = DISJOINT;
//
//	return distance;
//}

shared_ptr<Collision> Collision::CheckCollision(PhysicsComponentPtr component1, PhysicsComponentPtr component2)
{
	ColliderType colliderType1 = component1->ColliderBounding->Type;
	ColliderType colliderType2 = component2->ColliderBounding->Type;

	if (colliderType1 == AABB && colliderType2 == AABB)
	{
		ContainmentTypeVector collisionResult = Collide(dynamic_pointer_cast<ColliderAABB>(component1->ColliderBounding), dynamic_pointer_cast<ColliderAABB>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
		}
	}

	/*if (colliderType1 == Sphere && colliderType2 == Sphere)
	{
		ContainmentTypeVector collisionResult = Collide(dynamic_pointer_cast<ColliderSphere>(component1->ColliderBounding), dynamic_pointer_cast<ColliderSphere>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
		}
	}*/

	/*if (colliderType1 == AABB && colliderType2 == Sphere)
	{
		ContainmentTypeVector collisionResult = Collide(dynamic_pointer_cast<ColliderAABB>(component1->ColliderBounding), dynamic_pointer_cast<ColliderSphere>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
		}
	}

	if (colliderType1 == Sphere && colliderType2 == AABB)
	{
		ContainmentTypeVector collisionResult = Collide(dynamic_pointer_cast<ColliderSphere>(component1->ColliderBounding), dynamic_pointer_cast<ColliderAABB>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
		}
	}*/

	if (colliderType1 == Frustum && colliderType2 == AABB)
	{
		ContainmentTypeVector collisionResult = Collide(dynamic_pointer_cast<ColliderFrustum>(component1->ColliderBounding), dynamic_pointer_cast<ColliderAABB>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
		}
	}

	/*if (colliderType1 == Frustum && colliderType2 == Sphere)
	{
		ContainmentTypeVector collisionResult = Collide(dynamic_pointer_cast<ColliderFrustum>(component1->ColliderBounding), dynamic_pointer_cast<ColliderSphere>(component2->ColliderBounding));

		if (collisionResult[0] == INTERSECTS && collisionResult[1] == INTERSECTS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), INTERSECTS));
		}

		if (collisionResult[0] == CONTAINS)
		{
			return (make_shared<Collision>(component1->GetParent(), component2->GetParent(), CONTAINS));
		}

		if (collisionResult[1] == CONTAINS)
		{
			return (make_shared<Collision>(component2->GetParent(), component1->GetParent(), CONTAINS));
		}
	}*/

	return nullptr;
}

shared_ptr<Collision> Collision::CheckCollision(PhysicsComponentPtr component, ColliderRayPtr ray)
{
	//ColliderType colliderType = component->ColliderBounding->Type;

	float resultDistance;

	//if (colliderType == AABB)
	//{
		resultDistance = Collide(dynamic_pointer_cast<ColliderAABB>(component->ColliderBounding), ray);

		if (resultDistance > 0)
			return make_shared<Collision>(component->GetParent(), component->ColliderBounding->CollisionKind, resultDistance);
	//}
	
	/*if (colliderType == Sphere)
	{
		resultDistance = Collide(dynamic_pointer_cast<ColliderSphere>(component->ColliderBounding), ray);

		if (resultDistance > 0)
			return make_shared<Collision>(component->GetParent(), component->ColliderBounding->CollisionKind, resultDistance);
	}*/

	return nullptr;
}