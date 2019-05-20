#include "pch.h"
#include "NavMesh.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

NavMesh::NavMesh()
{

}

NavMesh::NavMesh(std::shared_ptr<Transform> transform)
{
	this->transform = transform;
	destination = dxmath::Vector3::Zero;
	speed = 30.f;
	isMoving = false;
	step = dxmath::Vector3::Zero;
}

NavMesh::~NavMesh()
{
}

void NavMesh::SetDestination(dxmath::Vector3 dest)
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
				localDestination = currentPath.front()->worldPosition;
				transform->SetPosition(start->worldPosition);
				currentPath.erase(currentPath.begin());
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
	}
}