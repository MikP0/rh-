#pragma once

#include "pch.h"
#include "Component.h"
#include "Coroutine.h"
#include "NavMesh.h"
#include "Entity.h"
#include "AudioComponent.h"	

class PlayerComponent : public Component
{
public:
	PlayerComponent();
	~PlayerComponent();

	int attackType;
	int vampireAbility;

	bool isHit;

	bool isDancing;
	bool isWalking;
	bool isNormalAttack;
	bool isPowerAttack;
	bool isRipAttack;
	bool isBiteAttack;

	bool isPlayerHealed;

	bool enemyClicked;
	std::shared_ptr<Entity> hittedEnemy;

	std::shared_ptr<Entity> targetedEnemy;		// entity -> enemyComponent
	

	const float playerBiteDistance = 1.0f;
	const float playerAttackDistance = 1.0f;
	
	const float playerHealthOrigin = 10.0f;
	std::shared_ptr<float> playerHealth;

	float playerSpeed;

	std::shared_ptr<NavMesh> navMesh;


	void AddPlayerHealth(float HealthRate);


	const float playerNormalAttackDamage = 1.0f;
	const float playerPoweAttackDamage = 3.0f;
	const float playerBiteAttackDamage = 2.0f;
	const float playerBiteAttackHealRate = 3.0f;
	const float playerRipAttackDamage = 4.0f;

	const float playerRipPlayerDamage = 2.0f;
	const float playerTeleportSwapDamage = 1.0f;

	const float playerAfterEnemyKilledHealRate = 2.0f;

	std::shared_ptr<AudioComponent> footstepAudio;
	std::shared_ptr<AudioComponent> swordAudio;
	std::shared_ptr<AudioComponent> damageAudio;
};

