#include "PlayerComponent.h"

PlayerComponent::PlayerComponent()
{
	attackType = 0;

	isDancing = false;
	isWalking = false;
	isNormalAttack = false;
	isBiteAttack = false;

	isPlayerHealed = false;

	enemyClicked = false;
	hittedEnemy = nullptr;
	targetedEnemy = nullptr;

	playerHealth = std::make_shared<float>(playerHealthOrigin);
	playerSpeed = 30.f;

	navMesh = nullptr;

	playerAttackCorutine.active = false;
	playerBiteCorutine.active = false;
	playerHittedCorutine.active = false;
	playerHealedCorutine.active = false;
}


PlayerComponent::~PlayerComponent()
{
}
