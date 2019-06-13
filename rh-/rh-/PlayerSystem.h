#pragma once
#include "pch.h"
#include "System.h"
#include "Entity.h"
#include "PlayerComponent.h"
#include "Terrain.h"
#include "NavMesh.h"
#include "Coroutine.h"
#include "RenderableComponent.h"

class PlayerSystem : public System
{
public:
	PlayerSystem();
	~PlayerSystem();

	virtual void Iterate() override;
	virtual void Initialize() override;
	void AdditionalInitialization(std::shared_ptr<Terrain> Terrain);

	std::shared_ptr<PlayerComponent> player;
	std::shared_ptr<float> playerHealth;
	float playerHealthOrigin;

	bool vampireMode;
	void SetVampireMode(bool mode);

	void GetMouseAndKeyboardStates(DirectX::Mouse mouse, DirectX::Keyboard keyboard);
	DirectX::Mouse mouseState;
	DirectX::Keyboard keyboardStates;
};

