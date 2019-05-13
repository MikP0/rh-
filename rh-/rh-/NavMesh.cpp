#include "pch.h"
#include "NavMesh.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

NavMesh::NavMesh() {

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

void NavMesh::SetDestination(dxmath::Vector3 dest) {
	if (dest != transform->GetPosition()) {
		destination = dest;
		isMoving = true;
		float dot = transform->GetTransformMatrix().Forward().x * (destination - transform->GetPosition()).x + transform->GetTransformMatrix().Forward().z * (destination - transform->GetPosition()).z;
		float cross = transform->GetTransformMatrix().Forward().x * (destination - transform->GetPosition()).z - transform->GetTransformMatrix().Forward().z * (destination - transform->GetPosition()).x;
		float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
		transform->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

		step = destination - transform->GetPosition();		
		step = step / step.Length()/speed;
		this->Move();
}
}


void NavMesh::Move() {
	//while (isMoving) {
		if (isMoving && !XMVector3NearEqual(destination, transform->GetPosition(), Vector3(.01f, .01f, .01f))) {
			
			if (terrain->CanWalk(transform->GetPosition() + step)) {
				transform->SetPosition(transform->GetPosition() + step);
			}
			else
			{
				isMoving = false;
			}
			//transform->SetPosition(transform->GetPosition() + step);
			//mSkinModel->GetAnimatorPlayer()->StartClip("Walk");
			//mSkinModel->SetInMove(true);
			//mSkinModel->GetAnimatorPlayer()->SetDirection(true);
		}
		else {
			isMoving = false;
		}
	//}
}

