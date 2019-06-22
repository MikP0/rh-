#pragma once

#include "pch.h"
#include "Component.h"
#include "Coroutine.h"
#include "NavMesh.h"
#include "Entity.h"
#include "AudioComponent.h"	
#include "RenderableComponent.h"

class PlayerComponent : public Component
{
public:
	PlayerComponent();
	~PlayerComponent();

	int attackType;
	int vampireAbility;

	bool isHit;
	bool isHealed;

	bool isDancing;
	bool isWalking;
	bool isNormalAttack;
	bool isPowerAttack;
	bool isRipAttack;
	bool isBiteAttack;
	bool isAOEAttack;
	bool isSpinAttack;


	bool isPlayerHealed;

	bool enemyClicked;
	std::shared_ptr<Entity> hittedEnemy;

	std::shared_ptr<Entity> targetedEnemy;		// entity -> enemyComponent
	

	const float playerBiteDistance = 1.0f;
	const float playerAttackDistance = 1.0f;
	
	const float playerHealthOrigin = 10.0f;
	std::shared_ptr<float> playerHealth;

	DirectX::SimpleMath::Vector3 playerPositionOrigin;

	float playerSpeed;

	std::shared_ptr<NavMesh> navMesh;


	void AddPlayerHealth(float HealthRate);

	void LoadPlayerAnimations();


	const float playerNormalAttackDamage = 1.0f;
	const float playerPoweAttackDamage = 3.0f;
	const float playerBiteAttackDamage = 2.0f;
	const float playerBiteAttackHealRate = 3.0f;
	const float playerRipAttackDamage = 4.0f;
	const float playerAOEAttackDamage = 3.0f;

	const float playerSpinAttackDamage = 2.0f;
	const float playerSpinDistance = 1.0f;
	DirectX::SimpleMath::Vector3 newPosToSpin;

	const float playerRipPlayerDamage = 2.0f;
	const float playerTeleportSwapDamage = 1.0f;
	const float playerAOEDamage = 3.0f;

	const float playerAfterEnemyKilledHealRate = 2.0f;

	const float playerTeleport_SwapDistance = 6.0f;

	const float playerAoEDistance = 3.0f;

	std::shared_ptr<AudioComponent> footstepAudio;
	std::shared_ptr<AudioComponent> normalAttackAudio;
	std::shared_ptr<AudioComponent> powerAttackAudio;
	std::shared_ptr<AudioComponent> spinAttackAudio;
	std::shared_ptr<AudioComponent> biteAudio;
	std::shared_ptr<AudioComponent> damageAudio;


	std::shared_ptr<AudioComponent> teleportAudio;
	std::shared_ptr<AudioComponent> ripAttackAudio;
	std::shared_ptr<AudioComponent> swapAudio;
	std::shared_ptr<AudioComponent> aoeAudio;
};

