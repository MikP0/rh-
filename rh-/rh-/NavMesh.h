#pragma once
#include "pch.h"
#include "Transform.h"

namespace dxmath = DirectX::SimpleMath;

class NavMesh //: public std::enable_shared_from_this<NavMesh>
{
public:
	NavMesh();
	NavMesh(std::shared_ptr<Transform>);
	virtual ~NavMesh();
	void Move();
	void SetDestination(dxmath::Vector3);

	bool isMoving;

private:
	std::shared_ptr<Transform> transform;
	dxmath::Vector3 destination;
	float speed;	
	dxmath::Vector3 step;
};