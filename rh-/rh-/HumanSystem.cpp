#include "pch.h"
#include "HumanSystem.h"
#include "AudioSystem.h"

HumanSystem::HumanSystem(std::shared_ptr<PhysicsSystem> collSys, Camera* cam)
{
	player = nullptr;
	playerHealth = nullptr;

	playerHealthOrigin = 0;

	collisionSystem = collSys;
	playerRenderableComponent = nullptr;
	camera = cam;

	stopHumanMode = false;
	stopInput = false;
}

HumanSystem::~HumanSystem()
{
}

void HumanSystem::Iterate()
{
	if (!stopInput)
	{
		if (humanMode)
		{
			keyboardTracker.Update(Input::GetKeyboardState());
			mouseTracker.Update(Input::GetMouseState());

			UpdateNormalMode();
			UpdateCorutines();
			UpdateAnimations();
		}
	}
}

void HumanSystem::Initialize()
{
	for (auto playerComponent : _world->GetComponents<HumanComponent>())
	{
		player = playerComponent;
		playerEntity = playerComponent->GetParent();
		playerHealth = playerComponent->playerHealth;
		playerHealthOrigin = playerComponent->playerHealthOrigin;
		playerRenderableComponent = playerEntity->GetComponent<RenderableComponent>();
	}
	InitializeCheckpoints();
}

void HumanSystem::InitializeCheckpoints()
{
	checkpointMap[1] = dmath::Vector3(2.0f, 0.0f, 15.0f);
	checkpointMap[2] = dmath::Vector3(10.0f, 0.0f, 60.0f);
}

void HumanSystem::AdditionalInitialization(std::shared_ptr<Terrain> Terrain, vector<string> humanSkillsNames, vector<string> vampireSkillsNames, vector<float> skillsTimeLimits, vector<bool> skillsBlockadeStates, std::shared_ptr<bool> mesgMode)
{
	player->navMesh = std::make_shared<NavMesh>(player->GetParent()->GetTransform());
	player->navMesh->terrain = Terrain;
	player->navMesh->speed = player->playerSpeed;
	player->playerPositionOrigin = player->GetParent()->GetTransform()->GetPosition();
	messageMode = mesgMode;
}

void HumanSystem::RespawnPlayer(int checkpoint)
{
	*player->playerHealth = player->playerHealthOrigin;
	playerEntity->GetTransform()->SetPosition(checkpointMap[checkpoint]);
}


void HumanSystem::UpdateNormalMode()
{
	if ((mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED || mouseTracker.leftButton == Mouse::ButtonStateTracker::HELD) && (*messageMode == false))
	{
		shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(*camera));
		vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

		for each (shared_ptr<Collision> coll in collisionsWithRay)
		{
			if (coll->OriginObject->GetName() == "SwordModel")
			{	
				if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), coll->OriginObject->GetTransform()->GetPosition(), Vector3(2.5f, 2.1f, 2.5f)))
				{
					coll->OriginObject->GetComponent<RenderableComponent>()->SetIsEnabled(false);
					stopHumanMode = true;
				}
			}
		}

		if (!stopHumanMode)
		{
			Vector3 destination = Raycast::GetPointOnGround(*camera);
			player->navMesh->SetDestination(destination);
			player->isWalking = true;

			player->enemyClicked = false;
			player->targetedEnemy = nullptr;
		}
	}


	if (player->isWalking)
	{
		player->navMesh->Move(Coroutine::elapsedTime);

		if (!player->navMesh->isMoving)
		{
			player->isWalking = false;
		}
	}
}

void HumanSystem::UpdateCorutines()
{

}

void HumanSystem::UpdateAnimations()
{
	if (player->isWalking)
	{
		if (player->footstepAudio != nullptr) {
			player->footstepAudio->Mute = false;
			if (player->footstepAudio->AudioLoopInstance->GetState() != SoundState::PLAYING) {
				player->footstepAudio->AudioFile->Play(player->footstepAudio->Volume*AudioSystem::VOLUME, player->footstepAudio->Pitch, player->footstepAudio->Pan);
			}
		}

		playerRenderableComponent->_modelSkinned->currentAnimation = "Walk";
	}
	else if (!player->isWalking)
	{
		playerRenderableComponent->_modelSkinned->currentAnimation = "Idle";
	}
}