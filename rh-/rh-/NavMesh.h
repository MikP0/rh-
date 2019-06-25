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
	NavMesh(std::shared_ptr<Transform>);
	virtual ~NavMesh();
	void SetDestination(dxmath::Vector3);
	void SetEnemyDestination(dxmath::Vector3);
	void RotateAndSetStep();
	void Move(float);
	void MoveEnemy(float, std::shared_ptr<PhysicsComponent>);


	bool isMoving;

	std::shared_ptr<Terrain> terrain;
	std::shared_ptr<Transform> transform;
	dxmath::Vector3 destination;
	dxmath::Vector3 localDestination;
	dxmath::Vector3 previousPosition;
	float speed;	
	dxmath::Vector3 step;
	std::vector<std::shared_ptr<MapTile>> currentPath;

	std::shared_ptr<PhysicsSystem> collisionSystem;
};