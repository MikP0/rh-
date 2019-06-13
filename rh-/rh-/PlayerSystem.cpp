#include "PlayerSystem.h"

PlayerSystem::PlayerSystem()
{
	player = nullptr;
	playerHealth = nullptr;

	vampireMode = false;

	playerHealthOrigin = 0;
}

PlayerSystem::~PlayerSystem()
{
}

void PlayerSystem::Iterate()
{
}

void PlayerSystem::Initialize()
{
	for (auto playerComponent : _world->GetComponents<PlayerComponent>())
	{
		player = playerComponent;
		playerHealth = playerComponent->playerHealth;
		playerHealthOrigin = playerComponent->playerHealthOrigin;
	}
}

void PlayerSystem::AdditionalInitialization(std::shared_ptr<Terrain> Terrain)
{
	player->navMesh = std::make_shared<NavMesh>(player->GetParent()->GetTransform());
	player->navMesh->terrain = Terrain;
	player->navMesh->speed = player->playerSpeed;
}

void PlayerSystem::SetVampireMode(bool mode)
{
	vampireMode = mode;
}

void PlayerSystem::GetMouseAndKeyboardStates(DirectX::Mouse mouse, DirectX::Keyboard keyboard)
{
	//mouseState = mouse;
	//keyboardStates = keyboard;
}
