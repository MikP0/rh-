#include "pch.h"
#include "EnemyComponent.h"


EnemyComponent::EnemyComponent()
{
	this->health = 5.f;
	this->originHealth = health;
	this->speed = 40.f;
	this->attackLength = 1.9f;
	this->damage = 1.f;
	this->distanceToAttack = 1.f;
	this->followPlayerDistance = 10.f;

	this->hit = false;
	this->bited = false;
	
	this->dying = false;

	this->navMesh = nullptr;

	this->attackCorutine.active = false;
	this->hitCorutine.active = false;
	this->dyingCorutine.active = false;
	this->hitColorCorutine.active = false;

	enemyState = EnemyState::IDLE;

	enemyRenderableComponent = nullptr;

	canBeHitted = true;
}

EnemyComponent::EnemyComponent(float Health, float Speed, float AttackLength, float AttackDamageTime, float Damage, float DistanceToAttack, float FollowPlayerDistance)
{
	this->health = Health;
	this->originHealth = this->health;
	this->speed = Speed;
	this->attackLength = AttackLength;
	this->damage = Damage;
	this->distanceToAttack = DistanceToAttack;
	this->followPlayerDistance = FollowPlayerDistance;
	this->attackDamageTime = AttackDamageTime;

	this->hit = false;
	this->bited = false;

	this->dying = false;

	this->navMesh = nullptr;

	this->attackCorutine.active = false;
	this->hitCorutine.active = false;
	this->dyingCorutine.active = false;
	this->hitColorCorutine.active = false;

	enemyState = EnemyState::IDLE;

	enemyRenderableComponent = nullptr;

	canBeHitted = true;
}


EnemyComponent::~EnemyComponent()
{
}