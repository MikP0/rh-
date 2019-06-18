#include "pch.h"
#include "PlayerComponent.h"

PlayerComponent::PlayerComponent()
{
	attackType = 0;
	vampireAbility = 0;

	isHit = false;

	isDancing = false;
	isWalking = false;
	isNormalAttack = false;
	isBiteAttack = false;
	isRipAttack = false;
	isPowerAttack = false;

	isPlayerHealed = false;

	enemyClicked = false;
	hittedEnemy = nullptr;
	targetedEnemy = nullptr;

	playerHealth = std::make_shared<float>(playerHealthOrigin);
	playerSpeed = 15.f;

	navMesh = nullptr;
}


PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::AddPlayerHealth(float HealthRate)
{
	*playerHealth += HealthRate;

	if (*playerHealth > playerHealthOrigin)
		* playerHealth = playerHealthOrigin;
}
