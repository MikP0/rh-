#pragma once
#include "pch.h"
#include "System.h"
#include "Entity.h"
#include "EnemyComponent.h"
#include "Terrain.h"
#include "NavMesh.h"
#include "Coroutine.h"
#include "RenderableComponent.h"
#include "PlayerComponent.h"

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

	bool CheckRangeAndCone(std::shared_ptr<EnemyComponent> enemy, DirectX::SimpleMath::Vector3 vecTar, float distance, float Cone);

	void SetStates(std::shared_ptr<EnemyComponent> enemy);
	void ApplyStates(std::shared_ptr<EnemyComponent> enemy);
	void CheckCorutines(std::shared_ptr<EnemyComponent> enemy);

	int RespawnEnemiesFromCheckpoint();

	bool humanMode;

	bool menuIsOn;

	bool stopInput;

	void BossMode(std::shared_ptr<EnemyComponent> boss);

	bool bossModeCheck;

	Coroutine boosIdleCorutine;
	Coroutine boosAttackCorutine;

	bool endGame;
};