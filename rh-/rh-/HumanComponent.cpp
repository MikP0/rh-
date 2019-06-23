#include "pch.h"
#include "HumanComponent.h"

HumanComponent::HumanComponent()
{
	enemyClicked = false;
	hittedEnemy = nullptr;
	targetedEnemy = nullptr;

	playerHealth = std::make_shared<float>(playerHealthOrigin);
	playerSpeed = 15.f;

	navMesh = nullptr;
}


HumanComponent::~HumanComponent()
{
}

void HumanComponent::LoadHumanAnimations()
{
	auto component = GetParent()->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Idle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Run_Slow.fbx", "Walk");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_NormalAttack.fbx", "Attack");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Bite.fbx", "Bite");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Rip.fbx", "Rip");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_PowerAttack_Normal.fbx", "PowerAttack");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_Spin_2H.fbx", "SpinAttack");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Anna_4th.fbx", "4th");
}
