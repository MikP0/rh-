#pragma once
#include "pch.h"
#include "Component.h"
#include "NavMesh.h"
#include "Entity.h"
#include "Coroutine.h"
#include "AudioComponent.h"
#include "RenderableComponent.h"

enum EnemyState
{
	IDLE = 1,
	FOLLOW = 2,
	ATTACK = 3,
	HIT = 4,
	BITED = 5,
	DYING = 6,
	DEAD = 0
};

class EnemyComponent : public Component
{
public:
	EnemyComponent();
	EnemyComponent(int checkpointNumber, float Health, float Speed = 20.0f, float AttackLength = 1.9f, float AttackDamageTime = 0.6f, float Damage = 1.f, float DistanceToAttack = 1.0f, float FollowPlayerDistance = 5.f);
	virtual ~EnemyComponent();

	void LoadBruteAnimations();
	void LoadGuardAnimations();
	void LoadBossAnimations();
	void LoadFewWarriorAnimations();

	bool isGuard;

	float health;
	float originHealth;
	float speed;

	DirectX::SimpleMath::Vector3 originPosition;

	float attackLength;
	float attackDamageTime;
	float damage;
	float distanceToAttack;

	std::shared_ptr<RenderableComponent> enemyRenderableComponent;

	std::shared_ptr<NavMesh> navMesh;

	float followPlayerDistance;

	bool hit;
	bool bited;

	bool canBeHitted;

	bool dying;

	Coroutine attackCorutine;

	Coroutine dyingCorutine;

	Coroutine hitCorutine;

	Coroutine hitColorCorutine;

	EnemyState enemyState;

	std::shared_ptr<AudioComponent> footstepAudio;
	std::shared_ptr<AudioComponent> normalAttackAudio;
	std::shared_ptr<AudioComponent> damageAudio;
	std::shared_ptr<AudioComponent> deathAudio;

	int checkpointNumber;

	bool isBoss;
};

