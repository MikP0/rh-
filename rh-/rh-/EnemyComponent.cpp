#include "EnemyComponent.h"


EnemyComponent::EnemyComponent()
{
	this->health = 100.f;
	this->originHealth = health;
	this->speed = 40.f;
	this->attackLength = 1.9f;
	this->damage = 10.f;
	this->distanceToAttack = 1.f;
	this->followPlayerDistance = 10.f;

	this->walking = false;
	this->attack = false;
	this->hit = false;
	this->bited = false;
	
	this->dying = false;

	this->navMesh = nullptr;

	this->attackCorutine.active = false;
	this->playerHittedCorutine.active = false;
	this->dyingCorutine.active = false;
}

EnemyComponent::EnemyComponent(float Health, float Speed, float AttackLength, float Damage, float DistanceToAttack, float FollowPlayerDistance)
{
	this->health = Health;
	this->originHealth = this->health;
	this->speed = Speed;
	this->attackLength = AttackLength;
	this->damage = Damage;
	this->distanceToAttack = DistanceToAttack;
	this->followPlayerDistance = FollowPlayerDistance;

	this->walking = false;
	this->attack = false;
	this->hit = false;
	this->bited = false;

	this->dying = false;

	this->navMesh = nullptr;

	this->attackCorutine.active = false;
	this->playerHittedCorutine.active = false;
	this->dyingCorutine.active = false;
}


EnemyComponent::~EnemyComponent()
{
}