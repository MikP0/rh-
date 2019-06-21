#include "pch.h"
#include "PlayerSystem.h"
#include "AudioSystem.h"

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

	if (player->isHealed)
		PlayerHealed();

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
	cooldown->Update();
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

void PlayerSystem::AdditionalInitialization(std::shared_ptr<Terrain> Terrain, vector<string> humanSkillsNames, vector<string> vampireSkillsNames, vector<float> skillsTimeLimits, vector<bool> skillsBlockadeStates)
{
	player->navMesh = std::make_shared<NavMesh>(player->GetParent()->GetTransform());
	player->navMesh->terrain = Terrain;
	player->navMesh->speed = player->playerSpeed;
	cooldown = make_shared<Cooldown>(humanSkillsNames, skillsTimeLimits);
	vector<string> skillsNames;
	skillsNames.insert(skillsNames.end(), humanSkillsNames.begin(), humanSkillsNames.end());
	skillsNames.insert(skillsNames.end(), vampireSkillsNames.begin(), vampireSkillsNames.end());
	blockade = make_shared<Blockade>(skillsNames, skillsBlockadeStates);
}

void PlayerSystem::PlayerHit()
{
	player->isHit = false;
	if (!playerHittedCorutine.active)
	{
		playerRenderableComponent->_modelSkinned->isHitted = true;
		playerHittedCorutine.Restart(0.1f);
	}
}

void PlayerSystem::PlayerHealed()
{
	player->isHealed = false;
	if (!playerHealedCorutine.active)
	{
		playerRenderableComponent->_modelSkinned->isHealed = true;
		playerHealedCorutine.Restart(0.1f);
	}
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
							if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying && cooldown->CanUseSkill("normalAttack") && !blockade->IsSkillBlocked("normalAttack"))
							{
								player->attackType = 1;
								player->enemyClicked = true;
								player->targetedEnemy = coll->OriginObject;
								cooldown->StartSkillCounter("normalAttack");
							}
						}
					}
					else
					{
						if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying && cooldown->CanUseSkill("normalAttack") && !blockade->IsSkillBlocked("normalAttack"))
						{
							player->attackType = 1;
							player->enemyClicked = true;
							player->targetedEnemy = coll->OriginObject;
							cooldown->StartSkillCounter("normalAttack");
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
						if (coll->OriginObject->GetName() != player->targetedEnemy->GetName() && cooldown->CanUseSkill("strongAttack") && !blockade->IsSkillBlocked("strongAttack"))
						{
							if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
							{
								player->attackType = 2;
								player->enemyClicked = true;
								player->targetedEnemy = coll->OriginObject;
								cooldown->StartSkillCounter("strongAttack");
							}
						}
					}
					else
					{
						if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying && cooldown->CanUseSkill("strongAttack") && !blockade->IsSkillBlocked("strongAttack"))
						{
							player->attackType = 2;
							player->enemyClicked = true;
							player->targetedEnemy = coll->OriginObject;
							cooldown->StartSkillCounter("strongAttack");
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
					if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying && cooldown->CanUseSkill("biteAttack") && !blockade->IsSkillBlocked("biteAttack"))
					{
						player->attackType = 5;
						player->enemyClicked = true;
						player->targetedEnemy = coll->OriginObject;
						cooldown->StartSkillCounter("biteAttack");
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
					//playerNormalAttackCorutine.Restart(1.5f);
					playerNormalAttackCorutine.RestartWithEvent(1.5f, 0.6f);
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
					//playerPowerAttackCorutine.Restart(1.3f);
					playerPowerAttackCorutine.RestartWithEvent(1.3f, 0.7f);
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

					player->biteAudio->AudioFile->Play(player->biteAudio->Volume*AudioSystem::VOLUME, player->biteAudio->Pitch, player->biteAudio->Pan);
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
			if (player->vampireAbility != 1 && !blockade->IsSkillBlocked("teleport"))
				player->vampireAbility = 1;
			else
				player->vampireAbility = 0;
		}

		if (player->vampireAbility == 1)
		{
			if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
			{
				Vector3 destination = Raycast::GetPointOnGround(*camera);

				if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), destination, Vector3(player->playerTeleport_SwapDistance, .1f, player->playerTeleport_SwapDistance)))
				{
					//bool freeWay = true;

					////shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(DirectX::SimpleMath::Vector3(playerEntity->GetTransform()->GetPosition().x, playerEntity->GetTransform()->GetPosition().y + 2.f, playerEntity->GetTransform()->GetPosition().z), destination));
					//XMVECTOR vec = { destination.x, destination.y, destination.z};
					//XMVECTOR playervec = { playerEntity->GetTransform()->GetPosition().x, playerEntity->GetTransform()->GetPosition().y, playerEntity->GetTransform()->GetPosition().z };
					//
					//XMVECTOR playNExtVec = DirectX::XMVector3Transform(Vector3::Zero, playerEntity->GetWorldMatrix());
					//

					//XMVECTOR origin = Vector4(playerEntity->GetTransform()->GetPosition().x, playerEntity->GetTransform()->GetPosition().y, playerEntity->GetTransform()->GetPosition().z, 0.0f);
					////XMFLOAT3 dirFromMouse = Raycast::GetRayDirFromMousePos(camera);
					//XMVECTOR direction = Vector4(destination.x, destination.y, destination.z, 0.0f);


					//std::shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(origin, direction));
					//vector<std::shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

					/*for each (std::shared_ptr<Collision> coll in collisionsWithRay)
					{
						if (coll->OriginObject->GetTag() == Tags::NONE)
						{
							if (coll->RayIntersectDist <= player->playerTeleport_SwapDistance)
							{
								freeWay = false;
							}
						}
					}*/

					/*for (int i = 0; i < collisionsWithRay.size(); i++)
					{
						if (collisionsWithRay[i]->OriginObject->GetTag() == Tags::NONE)
						{
							if (collisionsWithRay[i]->RayIntersectDist <= player->playerTeleport_SwapDistance)
							{
								int aa = 5;
								freeWay = false;
							}
						}
					}*/

					//if (freeWay)
					//{
						playerEntity->GetTransform()->SetPosition(destination);
						player->vampireAbility = 0;
						*playerHealth -= player->playerTeleportSwapDamage;

						PlayerHit();

						player->teleportAudio->AudioFile->Play(player->teleportAudio->Volume*AudioSystem::VOLUME, player->teleportAudio->Pitch, player->teleportAudio->Pan);
					//}
				}
			}
		}


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D2))
		{
			if (player->vampireAbility != 2 && !blockade->IsSkillBlocked("cleaveAttack"))
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
			if (player->vampireAbility != 3 && !blockade->IsSkillBlocked("swap"))
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
						if (!coll->OriginObject->GetComponent<EnemyComponent>()->dying)
						{
							player->enemyClicked = true;
							player->targetedEnemy = coll->OriginObject;
						}
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

					player->swapAudio->AudioFile->Play(player->swapAudio->Volume*AudioSystem::VOLUME, player->swapAudio->Pitch, player->swapAudio->Pan);
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
			if (!(playerNormalAttackCorutine.UpdateEvent()))
			{
				if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(1.5f, .1f, 1.5f)))
				{
					player->targetedEnemy->GetComponent<EnemyComponent>()->health -= player->playerNormalAttackDamage;
					player->targetedEnemy->GetComponent<EnemyComponent>()->hit = true;

					player->normalAttackAudio->AudioFile->Play(player->normalAttackAudio->Volume*AudioSystem::VOLUME, player->normalAttackAudio->Pitch, player->normalAttackAudio->Pan);
				}
			}

			if (!(playerNormalAttackCorutine.Update()))
			{
				player->isNormalAttack = false;

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->attackType = 0;
			}
		}

		if (playerPowerAttackCorutine.active)
		{
			if (!(playerPowerAttackCorutine.UpdateEvent()))
			{
				if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(1.5f, .1f, 1.5f)))
				{
					player->targetedEnemy->GetComponent<EnemyComponent>()->health -= player->playerPoweAttackDamage;
					player->targetedEnemy->GetComponent<EnemyComponent>()->hit = true;

					player->powerAttackAudio->AudioFile->Play(player->powerAttackAudio->Volume*AudioSystem::VOLUME, player->powerAttackAudio->Pitch, player->powerAttackAudio->Pan);
				}
			}

			if (!(playerPowerAttackCorutine.Update()))
			{
				player->isPowerAttack = false;

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

				player->ripAttackAudio->AudioFile->Play(player->ripAttackAudio->Volume*AudioSystem::VOLUME, player->ripAttackAudio->Pitch, player->ripAttackAudio->Pan);
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
					player->footstepAudio->AudioFile->Play(player->footstepAudio->Volume*AudioSystem::VOLUME, player->footstepAudio->Pitch, player->footstepAudio->Pan);
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
		if (player->targetedEnemy)
			player->targetedEnemy->GetComponent<EnemyComponent>()->bited = false;

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