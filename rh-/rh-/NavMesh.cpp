#include "pch.h"
#include "NavMesh.h"
#include <thread>
#include <chrono>
#include "ColliderTypes.h"
#include "Collision.h"
#include "Raycast.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

NavMesh::NavMesh()
{

}

NavMesh::NavMesh(std::shared_ptr<Transform> transform, std::shared_ptr<PhysicsSystem> collision)
{
	this->transform = transform;
	destination = dxmath::Vector3::Zero;
	speed = 30.f;
	isMoving = false;
	step = dxmath::Vector3::Zero;
	this->collisionSystem = collision;
}

NavMesh::~NavMesh()
{
}

void NavMesh::SetDestination(dxmath::Vector3 dest, Camera camera)
{
	if (dest != transform->GetPosition() && terrain->CanWalk(dest))
	{
		destination = dest;
		localDestination = dest;

		float dot = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).x + transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).z;
		float cross = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).z - transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).x;
		float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
		transform->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

		step = localDestination - transform->GetPosition();
		step = step / step.Length() / speed;

		isMoving = true;		
	}
}

void NavMesh::Move()
{
	if (isMoving)
	{
		if (!XMVector3NearEqual(localDestination, transform->GetPosition(), Vector3(.1f, .1f, .1f)))
		{
			if (terrain->CanWalk(transform->GetPosition() + step)) {
				transform->SetPosition(transform->GetPosition() + step);
			}
			else
			{
				shared_ptr<MapTile> start = terrain->GetTileWithPosition(transform->GetPosition());
				shared_ptr<MapTile> goal = terrain->GetTileWithPosition(destination);

				currentPath = terrain->GetPath(start, goal);
				//localDestination = currentPath.front()->worldPosition;
				//transform->SetPosition(start->worldPosition);
				//currentPath.erase(currentPath.begin());
				localDestination = start->worldPosition;
				float dot = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).x + transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).z;
				float cross = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).z - transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).x;
				float fAngle = (atan2(cross, dot) * 180.0f / 3.14159265f) + 180.0f;
				transform->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

				step = localDestination - transform->GetPosition();
				step = step / step.Length() / speed;
			}
		}
		else
		{
			if (!currentPath.empty())
			{
				localDestination = currentPath.front()->worldPosition;
				currentPath.erase(currentPath.begin());
				float dot = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).x + transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).z;
				float cross = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).z - transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).x;
				float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
				transform->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

				step = localDestination - transform->GetPosition();
				step = step / step.Length() / speed;
			}
			else
			{
				isMoving = false;
			}
		}
		XMVECTOR startPoint = DirectX::XMVector3Transform(Vector3::Zero, transform->GetTransformMatrix());
		Vector3 diff = localDestination - Vector3(startPoint);
		Vector3 direction = XMVector3Normalize(diff);

		XMVECTOR destPoint = Vector4(direction.x, direction.y, direction.z, 0.0f);
		shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(startPoint, destPoint));

		vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

		for each(shared_ptr<Collision> coll in collisionsWithRay)
		{
			Vector3 colpoint = DirectX::XMVector3Transform(Vector3::Zero, coll->OriginObject->GetWorldMatrix());
			terrain->MakeOcupied(colpoint);
			//coll->OriginObject
		}
	}
}