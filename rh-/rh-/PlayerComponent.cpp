#include "pch.h"
#include "PlayerComponent.h"

PlayerComponent::PlayerComponent()
{
	attackType = 0;
	vampireAbility = 0;

	isHit = false;
	isHealed = false;

	isDancing = false;
	isWalking = false;
	isNormalAttack = false;
	isBiteAttack = false;
	isRipAttack = false;
	isPowerAttack = false;
	isAOEAttack = false;
	isSpinAttack = false;

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

void PlayerComponent::LoadPlayerAnimations()
{
	auto component = GetParent()->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Idle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Run_Slow.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_NormalAttack.fbx", "Attack");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Bite.fbx", "Bite");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_RipAttack.fbx", "Rip");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_PowerAttack_Normal.fbx", "PowerAttack");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Spin_2H.fbx", "SpinAttack");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_4th.fbx", "4th");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_TheSword.fbx", "TheSword");
}
