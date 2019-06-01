#pragma once
#include "pch.h"
#include "Component.h"
#include "NavMesh.h"
#include "Entity.h"
#include "Coroutine.h"

class EnemyComponent : public Component
{
public:
	EnemyComponent();
	EnemyComponent(float Health, float Speed = 40.0f, float AttackLength = 1.9f, float Damage = 10.f, float DistanceToAttack = 1.f);
	virtual ~EnemyComponent();


	float health;
	float originHealth;
	float speed;

	float attackLength;
	float damage;
	float distanceToAttack;

	std::shared_ptr<NavMesh> navMesh;

	float followPlayerDistance = 10.f;
	bool walking;
	bool attack;
	bool hit;

	Coroutine attackCorutine;
};

