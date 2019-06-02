#pragma once
#include "pch.h"
#include "System.h"
#include "Entity.h"
#include "EnemyComponent.h"
#include "Terrain.h"
#include "NavMesh.h"
#include "Coroutine.h"
#include "RenderableComponent.h"

class EnemySystem : public System
{
public:
	EnemySystem();
	~EnemySystem();

	virtual void Iterate() override;
	virtual void Initialize() override;
	void AdditionalInitialization(std::shared_ptr<Entity> Player, std::shared_ptr<Terrain> Terrain, std::shared_ptr<float> PlayerHealth);

	std::shared_ptr<Entity> player;
	std::shared_ptr<float> playerHealth;

	bool vampireMode;
	void SetVampireMode(bool mode);
};

