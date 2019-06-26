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

	isGuard = false;

	checkpointNumber = 1;

	isBoss = false;
}

EnemyComponent::EnemyComponent(int checkpointNumber, float Health, float Speed, float AttackLength, float AttackDamageTime, float Damage, float DistanceToAttack, float FollowPlayerDistance)
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

	this->checkpointNumber = checkpointNumber;

	enemyState = EnemyState::IDLE;

	enemyRenderableComponent = nullptr;

	canBeHitted = true;

	isGuard = false;

	isBoss = false;
}


EnemyComponent::~EnemyComponent()
{
}

void EnemyComponent::LoadBruteAnimations()
{
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\BruteIdle.fbx", "Idle");
	enemyRenderableComponent->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\BruteRun.fbx", "Walk");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\BruteAttack.fbx", "Attack");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\BruteHit.fbx", "Hit");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\BruteDying.fbx", "Dying");
}

void EnemyComponent::LoadGuardAnimations()
{
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardIdle.fbx", "Idle");
	enemyRenderableComponent->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardWalk.fbx", "Walk");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardAttack.fbx", "Attack");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\EnemyGuardDying.fbx", "Dying");

	isGuard = true;
}

void EnemyComponent::LoadBossAnimations()
{
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\Boss_Idle.fbx", "Idle");
	enemyRenderableComponent->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\Boss_Run.fbx", "Run");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\Boss_Jump.fbx", "Jump");

	isBoss = true;
}

void EnemyComponent::LoadFewWarriorAnimations()
{
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\FemWarrior_Idle.fbx", "Idle");
	enemyRenderableComponent->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\FemWarrior_Walk.fbx", "Walk");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\FemWarrior_Attack.fbx", "Attack");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\FemWarrior_Dying.fbx", "Dying");
	enemyRenderableComponent->_modelSkinned->AddAnimationClip("content\\Models\\FemWarrior_Hit.fbx", "Hit");
}