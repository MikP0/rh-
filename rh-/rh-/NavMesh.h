#pragma once
#include "pch.h"
#include "Transform.h"
#include "Terrain.h"
#include "MapTile.h"
#include "PhysicsSystem.h"

namespace dxmath = DirectX::SimpleMath;

class NavMesh 
{
public:
	NavMesh();
	NavMesh(std::shared_ptr<Transform>, std::shared_ptr<PhysicsSystem>);
	virtual ~NavMesh();
	void Move();
	void SetDestination(dxmath::Vector3, Camera);


	bool isMoving;

	std::shared_ptr<Terrain> terrain;
	std::shared_ptr<Transform> transform;
	dxmath::Vector3 destination;
	dxmath::Vector3 localDestination;
	float speed;	
	dxmath::Vector3 step;
	std::vector<std::shared_ptr<MapTile>> currentPath;

	std::shared_ptr<PhysicsSystem> collisionSystem;
};