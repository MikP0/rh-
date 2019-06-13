#pragma once

#include "pch.h"
#include "Component.h"
#include "Coroutine.h"
#include "NavMesh.h"
#include "Entity.h"

class PlayerComponent : public Component
{
public:
	PlayerComponent();
	~PlayerComponent();

	int attackType;

	bool isDancing;
	bool isWalking;
	bool isNormalAttack;
	bool isBiteAttack;

	bool isPlayerHealed;

	bool enemyClicked;
	std::shared_ptr<Entity> hittedEnemy;

	std::shared_ptr<Entity> targetedEnemy;		// entity -> enemyComponent
	
	Coroutine playerAttackCorutine;
	Coroutine playerBiteCorutine;
	Coroutine playerHittedCorutine;
	Coroutine playerHealedCorutine;

	const float playerBiteDistance = 1.0f;
	const float playerAttackDistance = 1.0f;
	
	const float playerHealthOrigin = 100.0f;
	std::shared_ptr<float> playerHealth;

	float playerSpeed;

	std::shared_ptr<NavMesh> navMesh;
};

