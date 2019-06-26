#pragma once
#include "pch.h"
#include "System.h"
#include "Entity.h"
#include "HumanComponent.h"
#include "Terrain.h"
#include "NavMesh.h"
#include "Coroutine.h"
#include "RenderableComponent.h"
#include "Input.h"
#include "Raycast.h"
#include "Camera.h"
#include "EnemyComponent.h"
#include "Cooldown.h"
#include "Blockade.h"

namespace dmath = DirectX::SimpleMath;

class HumanSystem : public System
{
public:
	HumanSystem(std::shared_ptr<PhysicsSystem> collSys, Camera* cam);
	~HumanSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;
	void InitializeCheckpoints();
	void AdditionalInitialization(std::shared_ptr<Terrain> Terrain, vector<string> humanSkillsNames, vector<string> vampireSkillsNames, vector<float> skillsTimeLimits, vector<bool> skillsBlockadeStates, std::shared_ptr<bool> mesgMode);

	std::shared_ptr<PhysicsSystem> collisionSystem;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
	DirectX::Mouse::ButtonStateTracker mouseTracker;

	std::shared_ptr<bool> messageMode;
	std::shared_ptr<Entity> playerEntity;
	std::shared_ptr<HumanComponent> player;
	std::shared_ptr<RenderableComponent> playerRenderableComponent;
	std::shared_ptr<float> playerHealth;
	float playerHealthOrigin;

	void RespawnPlayer(int checkpoint);

	std::map<int, dmath::Vector3> checkpointMap;

	Camera* camera;

	bool humanMode;

	bool stopInput;

	bool stopHumanMode;

	void UpdateNormalMode();
	void UpdateCorutines();
	void UpdateAnimations();
};