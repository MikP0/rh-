#include "pch.h"
#include "PlayerSystem.h"

PlayerSystem::PlayerSystem(std::shared_ptr<PhysicsSystem> collSys, Camera* cam)
{
	player = nullptr;
	playerHealth = nullptr;

	vampireMode = false;

	playerHealthOrigin = 0;


	playerNormalAttackCorutine.active = false;
	playerPowerAttackCorutine.active = false;
	playerBiteCorutine.active = false;
	playerHittedCorutine.active = false;
	playerHealedCorutine.active = false;
	playerRipAttackCorutine.active = false;

	collisionSystem = collSys;
	playerRenderableComponent = nullptr;
	camera = cam;
}

PlayerSystem::~PlayerSystem()
{
}

void PlayerSystem::Iterate()
{
	keyboardTracker.Update(Input::GetKeyboardState());
	mouseTracker.Update(Input::GetMouseState());

	if (player->isHit)
		PlayerHit();

	if (!vampireMode)
	{
		UpdateNormalMode();
	}
	else
	{
		UpdateVampireMode();
	}

	UpdateCorutines();
	UpdateAnimations();
}

void PlayerSystem::Initialize()
{
	for (auto playerComponent : _world->GetComponents<PlayerComponent>())
	{
		player = playerComponent;
		playerEntity = playerComponent->GetParent();
		playerHealth = playerComponent->playerHealth;
		playerHealthOrigin = playerComponent->playerHealthOrigin;
		playerRenderableComponent = playerEntity->GetComponent<RenderableComponent>();
	}
}

void PlayerSystem::AdditionalInitialization(std::shared_ptr<Terrain> Terrain, std::shared_ptr<Cooldown> Cooldown)
{
	player->navMesh = std::make_shared<NavMesh>(player->GetParent()->GetTransform());
	player->navMesh->terrain = Terrain;
	player->navMesh->speed = player->playerSpeed;
	cooldown = Cooldown;
}

void PlayerSystem::PlayerHit()
{
	player->isHit = false;
	playerRenderableComponent->_modelSkinned->isHitted = true;
	playerHittedCorutine.Restart(0.1f);
}

void PlayerSystem::UpdateNormalMode()
{
	if (!playerBiteCorutine.active)
	{
		if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED || mouseTracker.leftButton == Mouse::ButtonStateTracker::HELD)
		{
			// !!!!!!!!!!!!!!!!    HOLD wylaczyc z atakowania - atakowanie tylko PRESSED

			shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(*camera));
			vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

			for each (shared_ptr<Collision> coll in collisionsWithRay)
			{
				if (coll->OriginObject->GetTag() == Tags::ENEMY)
				{
					if (player->targetedEnemy)
					{
						if (coll->OriginObject->GetName() != player->targetedEnemy->GetName())
						{
							if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
							{
								player->attackType = 1;
								player->enemyClicked = true;
								player->targetedEnemy = coll->OriginObject;
							}
						}
					}
					else
					{
						if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
						{
							player->attackType = 1;
							player->enemyClicked = true;
							player->targetedEnemy = coll->OriginObject;
						}
					}
				}
			}

			///////////////////////

			if ((!player->enemyClicked) || (collisionsWithRay.size() == 0))
			{
				Vector3 destination = Raycast::GetPointOnGround(*camera);
				player->navMesh->SetDestination(destination);
				player->isWalking = true;

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				playerNormalAttackCorutine.active = false;
				playerPowerAttackCorutine.active = false;
				playerBiteCorutine.active = false;
				player->isNormalAttack = false;
				player->isPowerAttack = false;
				player->isBiteAttack = false;
				player->attackType = 0;
			}
		}


		if (mouseTracker.rightButton == Mouse::ButtonStateTracker::PRESSED)
		{
			shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(*camera));
			vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

			for each (shared_ptr<Collision> coll in collisionsWithRay)
			{
				if (coll->OriginObject->GetTag() == Tags::ENEMY)
				{
					if (player->targetedEnemy)
					{
						if (coll->OriginObject->GetName() != player->targetedEnemy->GetName())
						{
							if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
							{
								player->attackType = 2;
								player->enemyClicked = true;
								player->targetedEnemy = coll->OriginObject;
							}
						}
					}
					else
					{
						if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
						{
							player->attackType = 2;
							player->enemyClicked = true;
							player->targetedEnemy = coll->OriginObject;
						}
					}
				}
			}

			///////////////////////

			if ((!player->enemyClicked) || (collisionsWithRay.size() == 0))
			{
				//Vector3 destination = Raycast::GetPointOnGround(*camera);
				//player->navMesh->SetDestination(destination);
				//player->isWalking = true;

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				playerNormalAttackCorutine.active = false;
				playerPowerAttackCorutine.active = false;
				playerBiteCorutine.active = false;
				player->isNormalAttack = false;
				player->isPowerAttack = false;
				player->isBiteAttack = false;
				player->attackType = 0;
			}
		}


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::E))
		{
			shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(*camera));
			vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

			for each (shared_ptr<Collision> coll in collisionsWithRay)
			{
				if (coll->OriginObject->GetTag() == Tags::ENEMY)
				{
					if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
					{
						player->attackType = 5;
						player->enemyClicked = true;
						player->targetedEnemy = coll->OriginObject;
					}
				}
				else
				{
					player->enemyClicked = false;
					player->targetedEnemy = nullptr;
					playerNormalAttackCorutine.active = false;
					playerPowerAttackCorutine.active = false;
					playerBiteCorutine.active = false;
					player->isNormalAttack = false;
					player->isPowerAttack = false;
					player->isBiteAttack = false;
					player->attackType = 0;
				}
			}

			///////////////////////

			if ((!player->enemyClicked) || (collisionsWithRay.size() == 0))
			{
				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				playerNormalAttackCorutine.active = false;
				playerPowerAttackCorutine.active = false;
				playerBiteCorutine.active = false;
				player->isNormalAttack = false;
				player->isPowerAttack = false;
				player->isBiteAttack = false;
				player->attackType = 0;
			}
		}
	}


	if (player->enemyClicked)
	{
		if ((!playerNormalAttackCorutine.active) && (!playerPowerAttackCorutine.active) && (!playerBiteCorutine.active))
		{
			if (player->attackType == 1)
			{
				if (!XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(player->playerAttackDistance, .1f, player->playerAttackDistance)))
				{
					player->navMesh->SetDestination(player->targetedEnemy->GetTransform()->GetPosition());
					player->isWalking = true;
				}
				else
				{
					player->navMesh->isMoving = false;
					player->isWalking = false;
					player->isNormalAttack = true;
					player->isBiteAttack = false;
					playerNormalAttackCorutine.Restart(1.5f);
				}
			}
			else if (player->attackType == 2)
			{
				if (!XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(player->playerAttackDistance, .1f, player->playerAttackDistance)))
				{
					player->navMesh->SetDestination(player->targetedEnemy->GetTransform()->GetPosition());
					player->isWalking = true;
				}
				else
				{
					player->navMesh->isMoving = false;
					player->isWalking = false;
					player->isPowerAttack = true;
					player->isBiteAttack = false;
					player->isNormalAttack = false;
					playerPowerAttackCorutine.Restart(1.3f);
				}
			}
			else if (player->attackType == 5)
			{
				if (!XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(player->playerBiteDistance, .1f, player->playerBiteDistance)))
				{
					player->navMesh->SetDestination(player->targetedEnemy->GetTransform()->GetPosition());
					player->isWalking = true;
				}
				else
				{
					player->navMesh->isMoving = false;
					player->isWalking = false;
					player->isNormalAttack = false;
					player->isBiteAttack = true;
					player->targetedEnemy->GetComponent<EnemyComponent>()->bited = true;
					playerBiteCorutine.Restart(4.1f);
				}
			}
		}
	}

	if (player->isWalking)
	{
		player->navMesh->Move(Coroutine::elapsedTime); // Player Component -> Player System

		if (!player->navMesh->isMoving)
		{
			player->isWalking = false;
		}
	}
}

void PlayerSystem::UpdateVampireMode()
{
	if (!playerRipAttackCorutine.active)
	{
		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D1))
		{
			if (player->vampireAbility != 1)
				player->vampireAbility = 1;
			else
				player->vampireAbility = 0;
		}

		if (player->vampireAbility == 1)
		{
			if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
			{
				Vector3 destination = Raycast::GetPointOnGround(*camera);
				playerEntity->GetTransform()->SetPosition(destination);
				player->vampireAbility = 0;
				*playerHealth -= player->playerTeleportSwapDamage;

				PlayerHit();
			}
		}


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D2))
		{
			if (player->vampireAbility != 2)
				player->vampireAbility = 2;
			else
				player->vampireAbility = 0;
		}

		if (player->vampireAbility == 2)
		{
			if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
			{
				shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(*camera));
				vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

				for each (shared_ptr<Collision> coll in collisionsWithRay)
				{
					if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), coll->OriginObject->GetTransform()->GetPosition(), Vector3(player->playerAttackDistance, .1f, player->playerAttackDistance)))
					{
						if (coll->OriginObject->GetTag() == Tags::ENEMY)
						{
							if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
							{
								player->enemyClicked = true;
								player->targetedEnemy = coll->OriginObject;
							}
						}
					}
				}

				if (player->enemyClicked)
				{
					player->isRipAttack = true;
					playerRipAttackCorutine.Restart(2.5f);
				}
			}
		}


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D3))
		{
			if (player->vampireAbility != 3)
				player->vampireAbility = 3;
			else
				player->vampireAbility = 0;
		}

		if (player->vampireAbility == 3)
		{
			if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
			{
				shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(*camera));
				vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

				for each (shared_ptr<Collision> coll in collisionsWithRay)
				{
					if (coll->OriginObject->GetTag() == Tags::ENEMY)
					{
						player->enemyClicked = true;
						player->targetedEnemy = coll->OriginObject;
					}
				}

				if (player->enemyClicked)
				{
					DirectX::SimpleMath::Vector3 tempVec = playerEntity->GetTransform()->GetPosition();
					playerEntity->GetTransform()->SetPosition(player->targetedEnemy->GetTransform()->GetPosition());
					player->targetedEnemy->GetTransform()->SetPosition(tempVec);

					*playerHealth -= player->playerTeleportSwapDamage;
					PlayerHit();

					player->enemyClicked = false;
					player->targetedEnemy = nullptr;
					player->vampireAbility = 0;
				}
			}
		}
	}
}

void PlayerSystem::UpdateCorutines()
{
	if (!vampireMode)
	{
		if (playerNormalAttackCorutine.active)
		{
			if (!(playerNormalAttackCorutine.Update()))
			{
				player->isNormalAttack = false;

				if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(1.5f, .1f, 1.5f)))
				{
					player->targetedEnemy->GetComponent<EnemyComponent>()->health -= player->playerNormalAttackDamage;
					player->targetedEnemy->GetComponent<EnemyComponent>()->hit = true;
					
					if (player->swordAudio->AudioLoopInstance->GetState() != SoundState::PLAYING) 
					{
						player->swordAudio->AudioFile->Play(player->swordAudio->Volume, player->swordAudio->Pitch, player->swordAudio->Pan);
					}
				}

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->attackType = 0;
			}
		}

		if (playerPowerAttackCorutine.active)
		{
			if (!(playerPowerAttackCorutine.Update()))
			{
				player->isPowerAttack = false;

				if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(1.5f, .1f, 1.5f)))
				{
					player->targetedEnemy->GetComponent<EnemyComponent>()->health -= player->playerPoweAttackDamage;
					player->targetedEnemy->GetComponent<EnemyComponent>()->hit = true;
					if (player->swordAudio->AudioLoopInstance->GetState() != SoundState::PLAYING)
					{
						player->swordAudio->AudioFile->Play(player->swordAudio->Volume, player->swordAudio->Pitch, player->swordAudio->Pan);
					}
				}

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->attackType = 0;
			}
		}

		if (playerBiteCorutine.active)
		{
			if (!(playerBiteCorutine.Update()))
			{
				player->isBiteAttack = false;

				player->targetedEnemy->GetComponent<EnemyComponent>()->bited = false;
				player->targetedEnemy->GetComponent<EnemyComponent>()->hit = true;
				player->targetedEnemy->GetComponent<EnemyComponent>()->health -= player->playerBiteAttackDamage;
				
				*playerHealth += player->playerBiteAttackHealRate;

				if (*playerHealth > playerHealthOrigin)
					* playerHealth = playerHealthOrigin;

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->attackType = 0;

				playerRenderableComponent->_modelSkinned->isHealed = true;
				playerHealedCorutine.Restart(0.1f);
			}
		}
	}
	else
	{
		if (playerRipAttackCorutine.active)
		{
			if (!(playerRipAttackCorutine.Update()))
			{
				player->isRipAttack = false;

				player->targetedEnemy->GetComponent<EnemyComponent>()->health -= player->playerRipAttackDamage;
				player->targetedEnemy->GetComponent<EnemyComponent>()->hit = true;

				
				*playerHealth -= player->playerRipPlayerDamage;
				PlayerHit();

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->vampireAbility = 0;
			}
		}
	}

	if (playerHittedCorutine.active)
	{
		if (!(playerHittedCorutine.Update()))
		{
			playerRenderableComponent->_modelSkinned->isHitted = false;
		}
	}

	if (playerHealedCorutine.active)
	{
		if (!(playerHealedCorutine.Update()))
		{
			playerRenderableComponent->_modelSkinned->isHealed = false;
		}
	}
}

void PlayerSystem::UpdateAnimations()
{
	if (vampireMode)
	{
		if (player->isRipAttack)
		{
			float dot = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x + playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z;
			float cross = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z - playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			playerEntity->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			playerRenderableComponent->_modelSkinned->currentAnimation = "Rip";
		}
		else
		{
			playerRenderableComponent->_modelSkinned->currentAnimation = "Idle";
		}
	}
	else
	{
		if (player->isWalking)
		{
			if (player->footstepAudio != nullptr) {
				player->footstepAudio->Mute = false;
				if (player->footstepAudio->AudioLoopInstance->GetState() != SoundState::PLAYING) {
					player->footstepAudio->AudioFile->Play(player->footstepAudio->Volume, player->footstepAudio->Pitch, player->footstepAudio->Pan);
					//player->footstepAudio->AudioLoopInstance->Play(true);
				}
			}
			
			playerRenderableComponent->_modelSkinned->currentAnimation = "Walk";
		}
		else if ((!player->isWalking) && (player->isNormalAttack))
		{
			float dot = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x + playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z;
			float cross = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z - playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			playerEntity->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			playerRenderableComponent->_modelSkinned->currentAnimation = "Attack";			
		}
		else if ((!player->isWalking) && (player->isPowerAttack))
		{
			float dot = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x + playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z;
			float cross = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z - playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			playerEntity->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			playerRenderableComponent->_modelSkinned->currentAnimation = "PowerAttack";			
		}
		else if ((!player->isWalking) && (player->isBiteAttack))
		{
			float dot = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x + playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z;
			float cross = playerEntity->GetTransform()->GetTransformMatrix().Forward().x * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).z - playerEntity->GetTransform()->GetTransformMatrix().Forward().z * (player->targetedEnemy->GetTransform()->GetPosition() - playerEntity->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			playerEntity->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			playerRenderableComponent->_modelSkinned->currentAnimation = "Bite";
		}
		else if ((!player->isWalking) && (!player->isNormalAttack) && (!player->isBiteAttack))
		{
			playerRenderableComponent->_modelSkinned->currentAnimation = "Idle";
		}
	}
}

void PlayerSystem::SetVampireMode(bool mode)
{
	vampireMode = mode;

	if (vampireMode)
	{
		player->enemyClicked = false;
		player->targetedEnemy = nullptr;
		playerNormalAttackCorutine.active = false;
		playerPowerAttackCorutine.active = false;
		playerBiteCorutine.active = false;
		player->isNormalAttack = false;
		player->isPowerAttack = false;
		player->isBiteAttack = false;
		player->isWalking = false;
		player->attackType = 0;
		player->navMesh->isMoving = false;
	}
	else
	{
		player->vampireAbility = 0;
		playerRipAttackCorutine.active = false;
		player->isRipAttack = false;
		playerRenderableComponent->_modelSkinned->currentAnimation = "Idle";
		player->enemyClicked = false;
		player->targetedEnemy = nullptr;
	}
}