#include "pch.h"
#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent(XMFLOAT3 extents)
{
	ColliderBounding = make_shared<ColliderAABB>(Vector3::Zero, extents);

}

PhysicsComponent::PhysicsComponent(Vector3 positionOffset, XMFLOAT3 extents)
{
	ColliderBounding = make_shared<ColliderAABB>(positionOffset, Vector3::Zero, extents);
	IsTriggered = false;
}

PhysicsComponent::PhysicsComponent(XMFLOAT3 extents, bool isTriggered)
{
	ColliderBounding = make_shared<ColliderAABB>(Vector3::Zero, extents);
	IsTriggered = false;
}

PhysicsComponent::PhysicsComponent(Vector3 positionOffset, XMFLOAT3 extents, bool isTriggered)
{
	ColliderBounding = make_shared<ColliderAABB>(positionOffset, Vector3::Zero, extents);
	IsTriggered = isTriggered;
}

//PhysicsComponent::PhysicsComponent(float radius)
//{
//	ColliderBounding = make_shared<ColliderSphere>(Vector3::Zero, radius);
//	IsTriggered = false;
//}
//
//PhysicsComponent::PhysicsComponent(Vector3 positionOffset, float radius)
//{
//	ColliderBounding = make_shared<ColliderSphere>(positionOffset, Vector3::Zero, radius);
//	IsTriggered = false;
//}
//
//PhysicsComponent::PhysicsComponent(float radius, bool isTriggered)
//{
//	ColliderBounding = make_shared<ColliderSphere>(Vector3::Zero, radius);
//	IsTriggered = isTriggered;
//}
//
//PhysicsComponent::PhysicsComponent(Vector3 positionOffset, float radius, bool isTriggered)
//{
//	ColliderBounding = make_shared<ColliderSphere>(positionOffset, Vector3::Zero, radius);
//	IsTriggered = isTriggered;
//}

PhysicsComponent::PhysicsComponent(shared_ptr<ColliderBase> collider)
{
	ColliderBounding = collider;
	IsTriggered = false;
}

PhysicsComponent::PhysicsComponent(shared_ptr<ColliderBase> collider, bool isTriggered)
{
	ColliderBounding = collider;
	IsTriggered = isTriggered;
}

PhysicsComponent::~PhysicsComponent()
{
}