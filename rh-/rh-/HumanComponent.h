#pragma once

#include "pch.h"
#include "Component.h"
#include "Coroutine.h"
#include "NavMesh.h"
#include "Entity.h"
#include "AudioComponent.h"	
#include "RenderableComponent.h"

class HumanComponent : public Component
{
public:
	HumanComponent();
	~HumanComponent();

	bool isWalking;
	
	bool enemyClicked;
	std::shared_ptr<Entity> hittedEnemy;
	std::shared_ptr<Entity> targetedEnemy;


	const float playerHealthOrigin = 1.0f;
	std::shared_ptr<float> playerHealth;

	DirectX::SimpleMath::Vector3 playerPositionOrigin;
	float playerSpeed;

	std::shared_ptr<NavMesh> navMesh;


	void LoadHumanAnimations();



	std::shared_ptr<AudioComponent> footstepAudio;
	std::shared_ptr<AudioComponent> damageAudio;
	std::shared_ptr<AudioComponent> aoeAudio;
};

