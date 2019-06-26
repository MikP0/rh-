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
#include "Blockade.h"

namespace dmath = DirectX::SimpleMath;

class PlayerSystem : public System
{
public:
	PlayerSystem(std::shared_ptr<PhysicsSystem> collSys, Camera* cam);
	~PlayerSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;
	void InitializeCheckpoints();
	void AdditionalInitialization(std::shared_ptr<Terrain> Terrain, vector<string> humanSkillsNames, vector<string> vampireSkillsNames, vector<float> skillsTimeLimits, vector<bool> skillsBlockadeStates, std::shared_ptr<bool> mesgMode);

	std::shared_ptr<PhysicsSystem> collisionSystem;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
	DirectX::Mouse::ButtonStateTracker mouseTracker;

	std::shared_ptr<Cooldown> cooldown;
	std::shared_ptr<Blockade> blockade;

	std::shared_ptr<bool> messageMode;
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
	Coroutine playerAOEAttackCorutine;
	Coroutine playerSpinAttackCorutine;

	Coroutine gettingWeaponCorutine;
	bool gettingWeapon;

	void RespawnPlayer(int checkpoint);

	std::vector<std::shared_ptr<Entity>> enemiesInRangeToAOE;

	std::map<int, dmath::Vector3> checkpointMap;

	Camera* camera;

	void PlayerHit();
	void PlayerHealed();
	

	bool turnOffVampireMode;


	void UpdateNormalMode();
	void UpdateVampireMode();
	void UpdateCorutines();
	void UpdateAnimations();

	bool humanMode;

	bool menuIsOn;

	bool stopInput;
};

