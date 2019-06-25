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

NavMesh::NavMesh(std::shared_ptr<Transform> transform)
{
	this->transform = transform;
	destination = dxmath::Vector3::Zero;
	localDestination = Vector3::Zero;
	previousPosition = Vector3::Zero;
	step = dxmath::Vector3::Zero;
	speed = 15.f;
	isMoving = false;
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
		RotateAndSetStep();
		currentPath.clear();
	}
}

void NavMesh::SetEnemyDestination(dxmath::Vector3 dest)
{
	if (dest != transform->GetPosition() && terrain->CanWalk(dest))
	{
		if (currentPath.empty()) {
			destination = dest;
			localDestination = dest;

			RotateAndSetStep();
		}
		else
		{
			isMoving = true;
		}

	}
}

void NavMesh::RotateAndSetStep()
{
	float dot = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).x + transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).z;
	float cross = transform->GetTransformMatrix().Forward().x * (localDestination - transform->GetPosition()).z - transform->GetTransformMatrix().Forward().z * (localDestination - transform->GetPosition()).x;
	float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
	transform->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

	step = localDestination - transform->GetPosition();
	step = step / step.Length() / speed;

	isMoving = true;
}

void NavMesh::Move(float deltaTime)
{
	if (isMoving)
	{
		MapTilePtr checkTile = terrain->GetTileWithPosition(localDestination);
		if (checkTile != nullptr && !checkTile->walkable) {
			localDestination = terrain->FallBack(transform->GetPosition(), previousPosition);
			RotateAndSetStep();
		}
		if (!XMVector3NearEqual(localDestination, transform->GetPosition(), Vector3(.1f, .1f, .1f)))
		{
			if (terrain->CanWalk(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)))) {				
				previousPosition = transform->GetPosition();
				transform->SetPosition(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)));
			}
			else
			{
				shared_ptr<MapTile> start = terrain->GetTileWithPosition(transform->GetPosition());
				shared_ptr<MapTile> goal = terrain->GetTileWithPosition(destination);

				currentPath = terrain->GetPath(start, goal);
				localDestination = start->worldPosition;
				RotateAndSetStep();
			}
		}
		else
		{
			if (!currentPath.empty())
			{
				localDestination = currentPath.front()->worldPosition;
				currentPath.erase(currentPath.begin());
				RotateAndSetStep();
			}
			else
			{
				isMoving = false;
			}
		}
	}
}

void NavMesh::MoveEnemy(float deltaTime, shared_ptr<PhysicsComponent> enemy)
{
	/*if (isMoving)
	{
		MapTilePtr checkTile = terrain->GetTileWithPosition(localDestination);
		if (checkTile != nullptr && !checkTile->walkable) {
			localDestination = terrain->FallBack(transform->GetPosition(), previousPosition);
			RotateAndSetStep();
		}
		if (!XMVector3NearEqual(localDestination, transform->GetPosition(), Vector3(.1f, .1f, .1f)))
		{
			if (terrain->CanMove(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)), enemy) == 1) {
				previousPosition = transform->GetPosition();
				transform->SetPosition(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)));
			}
			else if (terrain->CanMove(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)), enemy) == 2)
			{
				previousPosition = transform->GetPosition();
				MapTilePtr temp = terrain->GetTileWithPosition(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)));
				if (temp != nullptr)
				{
					//localDestination =terrain->GetNearestNeighbor(transform->GetPosition(), temp);
					//RotateAndSetStep();
					//currentPath.clear();
					shared_ptr<MapTile> start = terrain->GetTileWithPosition(transform->GetPosition());
					shared_ptr<MapTile> goal = terrain->GetTileWithPosition(terrain->GetNearestNeighbor(transform->GetPosition(), temp, enemy));

					currentPath = terrain->GetPath(start, goal);
					localDestination = start->worldPosition;
					RotateAndSetStep();
				}				
			}
			else
			{
				shared_ptr<MapTile> start = terrain->GetTileWithPosition(transform->GetPosition());
				shared_ptr<MapTile> goal = terrain->GetTileWithPosition(destination);

				currentPath = terrain->GetPath(start, goal);
				localDestination = start->worldPosition;
				RotateAndSetStep();
			}
		}
		else
		{
			if (!currentPath.empty())
			{
				localDestination = currentPath.front()->worldPosition;
				currentPath.erase(currentPath.begin());
				RotateAndSetStep();
			}
			else
			{
				isMoving = false;
			}
		}
	}*/
	if (isMoving)
	{
		MapTilePtr checkTile = terrain->GetTileWithPosition(localDestination);
		if (checkTile != nullptr && !checkTile->walkable) {
			localDestination = terrain->FallBack(transform->GetPosition(), previousPosition);
			RotateAndSetStep();
		}
		if (!XMVector3NearEqual(localDestination, transform->GetPosition(), Vector3(.1f, .1f, .1f)))
		{
			//if (terrain->CanWalk(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)))) {
			if (terrain->CanMove(transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime)), enemy)) {
				previousPosition = transform->GetPosition();
				terrain->ReleaseTile(terrain->GetTileWithPosition(previousPosition));
				Vector3 a = transform->GetPosition() + (step*Vector3(50.f*deltaTime, deltaTime, 50.f*deltaTime));
				transform->SetPosition(a);
				terrain->OccupyTile(terrain->GetTileWithPosition(a));
			}
			else
			{
				shared_ptr<MapTile> start = terrain->GetTileWithPosition(transform->GetPosition());
				shared_ptr<MapTile> goal = terrain->GetTileWithPosition(destination);

				currentPath = terrain->GetPath(start, goal);
				localDestination = start->worldPosition;
				RotateAndSetStep();
			}
		}
		else
		{
			if (!currentPath.empty())
			{
				localDestination = currentPath.front()->worldPosition;
				currentPath.erase(currentPath.begin());
				RotateAndSetStep();
			}
			else
			{
				isMoving = false;
			}
		}
	}
}