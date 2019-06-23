#include "pch.h"
#include "HumanComponent.h"

HumanComponent::HumanComponent()
{
	enemyClicked = false;
	hittedEnemy = nullptr;
	targetedEnemy = nullptr;

	playerHealth = std::make_shared<float>(playerHealthOrigin);
	playerSpeed = 25.f;

	navMesh = nullptr;
}


HumanComponent::~HumanComponent()
{
}

void HumanComponent::LoadHumanAnimations()
{
	auto component = GetParent()->GetComponent<RenderableComponent>();
	component->_modelSkinned->AddAnimationClip("content\\Models\\Human_Idle.fbx", "Idle");
	component->_modelSkinned->GetAnimatorPlayer()->StartClip("Idle");
	component->_modelSkinned->AddAnimationClip("content\\Models\\Human_Walk.fbx", "Walk");
}
