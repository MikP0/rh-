#pragma once
#include "pch.h"
#include "System.h"
#include "Entity.h"
#include "EnemyComponent.h"
#include "Terrain.h"
#include "NavMesh.h"

class EnemySystem : public System
{
public:
	EnemySystem();
	~EnemySystem();

	virtual void Iterate() override;
	virtual void Initialize() override;
	void MyInitialization(std::shared_ptr<Entity> Player, std::shared_ptr<Terrain> Terrain);

	float elapsedTime;
	void setElapsedTime(float Time);

	std::shared_ptr<Entity> player;
	void setPlayer(std::shared_ptr<Entity> Player);
};

