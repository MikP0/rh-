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
	if (dest != transform->GetPosition())
	{
		shared_ptr<MapTile> start = terrain->GetTileWithPosition(transform->GetPosition());
		shared_ptr<MapTile> goal = terrain->GetTileWithPosition(dest);

		currentPath = terrain->GetPath(start, goal);

		destination = dest;

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

			isMoving = true;
		}
	}
}


void NavMesh::Move()
{
	if (isMoving) 
	{
		if (!XMVector3NearEqual(localDestination, transform->GetPosition(), Vector3(.1f, .1f, .1f)))
		{
			transform->SetPosition(transform->GetPosition() + step);
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
	//transform->SetPosition(currentPath.front()->worldPosition);
	//currentPath.erase(currentPath.begin());

	//if (isMoving && !XMVector3NearEqual(localDestination, transform->GetPosition(), Vector3(.1f, .1f, .1f))) {
	//	if (terrain->CanWalk(transform->GetPosition() + step)) {
	//		transform->SetPosition(transform->GetPosition() + step);
	//	}
	//	else
	//	{
	//		if (!isMoving && !currentPath.empty()) {
	//			localDestination = currentPath.front()->worldPosition;
				//float dot = transform->GetTransformMatrix().Forward().x * (currentPath.front()->worldPosition - transform->GetPosition()).x + transform->GetTransformMatrix().Forward().z * (currentPath.front()->worldPosition - transform->GetPosition()).z;
				//float cross = transform->GetTransformMatrix().Forward().x * (currentPath.front()->worldPosition - transform->GetPosition()).z - transform->GetTransformMatrix().Forward().z * (currentPath.front()->worldPosition - transform->GetPosition()).x;
				//float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
				//transform->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

	//			step = localDestination - transform->GetPosition();
	//			step = step / step.Length() / speed;

	//			currentPath.erase(currentPath.begin());
				//isMoving = true;
	//		}
	//	}
	//}
	//else {
	//	isMoving = false;
	//}


	//while (isMoving) {
	//if (isMoving && !XMVector3NearEqual(destination, transform->GetPosition(), Vector3(.01f, .01f, .01f))) {

	//	if (terrain->CanWalk(transform->GetPosition() + step)) {
	//		transform->SetPosition(transform->GetPosition() + step);
	//	}
	//	else
	//	{
	//		isMoving = false;
	//	}
	//	//transform->SetPosition(transform->GetPosition() + step);
	//	//mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
	//	//mSkinModel->SetInMove(true);
	//	//mSkinModel->GetAnimatorPlayer()->SetDirection(true);
	//}
	//else {
	//	isMoving = false;
	//}
	//}
}