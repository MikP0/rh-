#pragma once
#include "pch.h"
#include "System.h"
#include "Entity.h"
#include "PlayerComponent.h"
#include "Terrain.h"
#include "NavMesh.h"
#include "Coroutine.h"
#include "RenderableComponent.h"
#include "Input.h"
#include "Raycast.h"
#include "Camera.h"
#include "EnemyComponent.h"
#include "RenderableComponent.h"
#include "Cooldown.h"

class PlayerSystem : public System
{
public:
	PlayerSystem(std::shared_ptr<PhysicsSystem> collSys, Camera* cam);
	~PlayerSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;
	void AdditionalInitialization(std::shared_ptr<Terrain> Terrain, std::shared_ptr<Cooldown> Cooldown);

	std::shared_ptr<PhysicsSystem> collisionSystem;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
	DirectX::Mouse::ButtonStateTracker mouseTracker;

	std::shared_ptr<Cooldown> cooldown;
	std::shared_ptr<Entity> playerEntity;
	std::shared_ptr<PlayerComponent> player;
	std::shared_ptr<RenderableComponent> playerRenderableComponent;
	std::shared_ptr<float> playerHealth;
	float playerHealthOrigin;

	bool vampireMode;
	void SetVampireMode(bool mode);

	Coroutine playerNormalAttackCorutine;
	Coroutine playerPowerAttackCorutine;
	Coroutine playerBiteCorutine;
	Coroutine playerHittedCorutine;
	Coroutine playerHealedCorutine;
	Coroutine playerRipAttackCorutine;

	Camera* camera;

	void PlayerHit();
	void PlayerHealed();
	


	void UpdateNormalMode();
	void UpdateVampireMode();
	void UpdateCorutines();
	void UpdateAnimations();
};

