#include "EnemyComponent.h"


EnemyComponent::EnemyComponent()
{
	this->health = 100.f;
	this->originHealth = health;
	this->speed = 40.f;
	this->attackLength = 1.9f;
	this->damage = 10.f;
	this->distanceToAttack = 1.f;

	this->walking = false;
	this->attack = false;
	this->hit = false;

	this->navMesh = nullptr;
}

EnemyComponent::EnemyComponent(float Health, float Speed , float AttackLength, float Damage, float DistanceToAttack)
{
	this->health = health;
	this->originHealth = this->health;
	this->speed = speed;
	this->attackLength = AttackLength;
	this->damage = Damage;
	this->distanceToAttack = DistanceToAttack;

	this->walking = false;
	this->attack = false;
	this->hit = false;

	this->navMesh = nullptr;
}


EnemyComponent::~EnemyComponent()
{
}