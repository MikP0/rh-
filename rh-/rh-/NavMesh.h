#pragma once
#include "pch.h"
#include "Transform.h"
#include "Terrain.h"
#include "MapTile.h"

namespace dxmath = DirectX::SimpleMath;

class NavMesh 
{
public:
	NavMesh();
	NavMesh(std::shared_ptr<Transform>);
	virtual ~NavMesh();
	void Move();
	void SetDestination(dxmath::Vector3);


	bool isMoving;

	std::shared_ptr<Terrain> terrain;
	std::shared_ptr<Transform> transform;
	dxmath::Vector3 destination;
	float speed;	
	dxmath::Vector3 step;
};