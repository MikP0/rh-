#pragma once
#include "pch.h"
#include "Component.h"
#include "NavMesh.h"
#include "Entity.h"
#include "Coroutine.h"
#include "AudioComponent.h"

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
	EnemyComponent(float Health, float Speed = 20.0f, float AttackLength = 1.9f, float Damage = 1.f, float DistanceToAttack = 1.0f, float FollowPlayerDistance = 10.f);
	virtual ~EnemyComponent();


	float health;
	float originHealth;
	float speed;

	float attackLength;
	float damage;
	float distanceToAttack;

	std::shared_ptr<RenderableComponent> enemyRenderableComponent;

	std::shared_ptr<NavMesh> navMesh;

	float followPlayerDistance;

	bool hit;
	bool bited;

	bool dying;

	Coroutine attackCorutine;

	Coroutine dyingCorutine;

	Coroutine hitCorutine;

	Coroutine hitColorCorutine;

	EnemyState enemyState;

	std::shared_ptr<AudioComponent> footstepAudio;
};

