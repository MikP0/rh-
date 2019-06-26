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
	playerAOEAttackCorutine.active = false;
	playerSpinAttackCorutine.active = false;

	gettingWeaponCorutine.active = false;

	collisionSystem = collSys;
	playerRenderableComponent = nullptr;
	camera = cam;

	turnOffVampireMode = false;

	gettingWeapon = false;

	menuIsOn = false;

	stopInput = false;
}

PlayerSystem::~PlayerSystem()
{
}

void PlayerSystem::Iterate()
{
	if ((!menuIsOn) && (!stopInput))
	{
		if (!humanMode)
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
	}
	else if (stopInput)
	{
		playerRenderableComponent->_modelSkinned->currentAnimation = "Idle";
	}
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
	InitializeCheckpoints();
}

void PlayerSystem::InitializeCheckpoints()
{
	checkpointMap[1] = dmath::Vector3(2.0f, 0.0f, 15.0f);
	checkpointMap[2] = dmath::Vector3(-7.7f, 0.0f, 59.4f);
	checkpointMap[3] = dmath::Vector3(-13.93f, 0, 70.26f);
	checkpointMap[4] = dmath::Vector3(-12.67f, 0, 91.79f);
}

void PlayerSystem::AdditionalInitialization(std::shared_ptr<Terrain> Terrain, vector<string> humanSkillsNames, vector<string> vampireSkillsNames, vector<float> skillsTimeLimits, vector<bool> skillsBlockadeStates, std::shared_ptr<bool> mesgMode)
{
	player->navMesh = std::make_shared<NavMesh>(player->GetParent()->GetTransform());
	player->navMesh->terrain = Terrain;
	player->navMesh->speed = player->playerSpeed;
	cooldown = make_shared<Cooldown>(humanSkillsNames, skillsTimeLimits);
	vector<string> skillsNames;
	skillsNames.insert(skillsNames.end(), humanSkillsNames.begin(), humanSkillsNames.end());
	skillsNames.insert(skillsNames.end(), vampireSkillsNames.begin(), vampireSkillsNames.end());
	blockade = make_shared<Blockade>(skillsNames, skillsBlockadeStates);
	player->playerPositionOrigin = player->GetParent()->GetTransform()->GetPosition();
	messageMode = mesgMode;
}

void PlayerSystem::RespawnPlayer(int checkpoint)
{
	//for (auto playerComponent : _world->GetComponents<PlayerComponent>())
	//{
		*player->playerHealth = player->playerHealthOrigin;
		player->GetParent()->GetTransform()->SetPosition(checkpointMap[checkpoint]);
		player->enemyClicked = false;
		player->targetedEnemy = nullptr;
		playerNormalAttackCorutine.active = false;
		playerPowerAttackCorutine.active = false;
		playerBiteCorutine.active = false;
		playerSpinAttackCorutine.active = false;
		enemiesInRangeToAOE.clear();
		player->isNormalAttack = false;
		player->isPowerAttack = false;
		player->isBiteAttack = false;
		player->isSpinAttack = false;
		player->isWalking = false;
		player->attackType = 0;
		player->navMesh->currentPath.clear();
	//}
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
		if ((mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED || mouseTracker.leftButton == Mouse::ButtonStateTracker::HELD) && (*messageMode == false))
		{
			if (cooldown->CanUseSkill("normalAttack") && !blockade->IsSkillBlocked("normalAttack"))
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
									player->attackType = 1;
									player->enemyClicked = true;
									player->targetedEnemy = coll->OriginObject;
									//cooldown->StartSkillCounter("normalAttack");
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
								//cooldown->StartSkillCounter("normalAttack");
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
					playerSpinAttackCorutine.active = false;
					player->isNormalAttack = false;
					player->isPowerAttack = false;
					player->isBiteAttack = false;
					player->attackType = 0;
				}
			}
		}


		if (mouseTracker.rightButton == Mouse::ButtonStateTracker::PRESSED && (*messageMode == false))
		{
			if (cooldown->CanUseSkill("strongAttack") && !blockade->IsSkillBlocked("strongAttack"))
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
									//cooldown->StartSkillCounter("strongAttack");
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
								//cooldown->StartSkillCounter("strongAttack");
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
					playerSpinAttackCorutine.active = false;
					playerBiteCorutine.active = false;
					player->isNormalAttack = false;
					player->isPowerAttack = false;
					player->isBiteAttack = false;
					player->attackType = 0;
				}
			}
		}


		if (mouseTracker.middleButton == Mouse::ButtonStateTracker::PRESSED && (*messageMode == false))
		{
			if (cooldown->CanUseSkill("spinAttack") && !blockade->IsSkillBlocked("spinAttack"))
			{
				shared_ptr<ColliderRay> sharedRay(Raycast::CastRay(*camera));
				vector<shared_ptr<Collision>> collisionsWithRay = collisionSystem->GetCollisionsWithRay(sharedRay);

				if (collisionsWithRay.size() > 0)
					player->newPosToSpin = Vector3(collisionsWithRay[0]->OriginObject->GetTransform()->GetPosition().x, playerEntity->GetTransform()->GetPosition().y, collisionsWithRay[0]->OriginObject->GetTransform()->GetPosition().z);
				else
					player->newPosToSpin = playerEntity->GetTransform()->GetPosition();

				player->enemyClicked = true;
				player->attackType = 3;
				//cooldown->StartSkillCounter("spinAttack");

				enemiesInRangeToAOE.clear();

				for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
				{
					if (enemyComponent->_isEnabled)
					{
						if (!enemyComponent->dying)
						{
							if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), enemyComponent->GetParent()->GetTransform()->GetPosition(), Vector3(player->playerSpinDistance, .1f, player->playerSpinDistance)))
							{
								enemiesInRangeToAOE.push_back(enemyComponent->GetParent());
							}
						}
					}
				}
			}
		}


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::E) && (*messageMode == false))
		{
			if (cooldown->CanUseSkill("biteAttack"))
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
							//cooldown->StartSkillCounter("biteAttack");
						}
					}
					else
					{
						player->enemyClicked = false;
						player->targetedEnemy = nullptr;
						playerNormalAttackCorutine.active = false;
						playerPowerAttackCorutine.active = false;
						playerSpinAttackCorutine.active = false;
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
	}


	if (player->enemyClicked)
	{
		if ((!playerNormalAttackCorutine.active) && (!playerPowerAttackCorutine.active) && (!playerBiteCorutine.active) && (!playerSpinAttackCorutine.active) && (!gettingWeaponCorutine.active))
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
					player->isSpinAttack = false;
					//playerNormalAttackCorutine.Restart(1.5f);
					playerNormalAttackCorutine.RestartWithEvent(1.5f, 0.6f);
					cooldown->StartSkillCounter("normalAttack");
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
					player->isSpinAttack = false;
					//playerPowerAttackCorutine.Restart(1.3f);
					playerPowerAttackCorutine.RestartWithEvent(1.3f, 0.7f);
					cooldown->StartSkillCounter("strongAttack");
				}
			}
			else if (player->attackType == 3)
			{
				player->navMesh->SetDestination(player->newPosToSpin);

				player->isSpinAttack = true;
				player->navMesh->isMoving = true;
				player->isWalking = false;
				player->isPowerAttack = false;
				player->isBiteAttack = false;
				player->isNormalAttack = false;

				if (enemiesInRangeToAOE.size() > 0)
				{
					for each (std::shared_ptr<Entity> ene in enemiesInRangeToAOE)
					{
						ene->GetComponent<EnemyComponent>()->health -= player->playerAOEAttackDamage / 2.0f;
						ene->GetComponent<EnemyComponent>()->hit = true;
					}
					//player->powerAttackAudio->AudioFile->Play(player->powerAttackAudio->Volume*AudioSystem::VOLUME, player->powerAttackAudio->Pitch, player->powerAttackAudio->Pan);
				}
				//enemiesInRangeToAOE.clear();

				playerSpinAttackCorutine.RestartWithEvent(2.0f, 0.2f);
				cooldown->StartSkillCounter("spinAttack");
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
					player->isSpinAttack = false;
					player->targetedEnemy->GetComponent<EnemyComponent>()->bited = true;
					playerBiteCorutine.Restart(4.1f);
					cooldown->StartSkillCounter("biteAttack");

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
	if ((!playerRipAttackCorutine.active) && (!playerAOEAttackCorutine.active))
	{
		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D1) && (*messageMode == false) && (*playerHealth >= player->playerTeleportSwapDamage + 1.0f))
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
					if (player->navMesh->terrain->CanWalk(destination))
					{
						playerEntity->GetTransform()->SetPosition(destination);
						player->vampireAbility = 0;
						*playerHealth -= player->playerTeleportSwapDamage;

						PlayerHit();

						player->teleportAudio->AudioFile->Play(player->teleportAudio->Volume*AudioSystem::VOLUME, player->teleportAudio->Pitch, player->teleportAudio->Pan);

					}
				}
			}
		}


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D2) && (*messageMode == false) && (*playerHealth >= player->playerRipPlayerDamage + 1.0f))
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
					//playerRipAttackCorutine.Restart(2.5f);
					playerRipAttackCorutine.RestartWithEvent(1.5f, 0.6f);
				}
			}
		}


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D3) && (*messageMode == false) && (*playerHealth >= player->playerTeleportSwapDamage + 1.0f))
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


		if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D4) && (*messageMode == false) && (*playerHealth >= player->playerAOEDamage + 1.0f))
		{
			if (player->vampireAbility != 4 && !blockade->IsSkillBlocked("aoeAttack"))
				player->vampireAbility = 4;
			else
				player->vampireAbility = 0;
		}

		if (player->vampireAbility == 4)
		{
			if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
			{
				enemiesInRangeToAOE.clear();

				for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
				{
					if (enemyComponent->_isEnabled)
					{
						if (!enemyComponent->dying)
						{
							if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), enemyComponent->GetParent()->GetTransform()->GetPosition(), Vector3(player->playerAoEDistance, .1f, player->playerAoEDistance)))
							{
								enemiesInRangeToAOE.push_back(enemyComponent->GetParent());
							}
						}
					}
				}

				if (enemiesInRangeToAOE.size() > 0)
				{
					player->isAOEAttack = true;
					playerAOEAttackCorutine.Restart(2.5f);
					player->aoeAudio->AudioFile->Play(player->aoeAudio->Volume*AudioSystem::VOLUME, player->aoeAudio->Pitch, player->aoeAudio->Pan);
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

		if (playerSpinAttackCorutine.active)
		{
			if (!(playerSpinAttackCorutine.UpdateEvent()))
			{
				/*if (enemiesInRangeToAOE.size() > 0)
				{
					enemiesInRangeToAOE.clear();
				}*/
				player->spinAttackAudio->AudioFile->Play(player->spinAttackAudio->Volume*AudioSystem::VOLUME, player->spinAttackAudio->Pitch, player->spinAttackAudio->Pan);
			}

			if (!(playerSpinAttackCorutine.Update()))
			{
				player->isSpinAttack = false;

				enemiesInRangeToAOE.clear();

				for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
				{
					if (enemyComponent->_isEnabled)
					{
						if (!enemyComponent->dying)
						{
							if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), enemyComponent->GetParent()->GetTransform()->GetPosition(), Vector3(player->playerSpinDistance, .1f, player->playerSpinDistance)))
							{
								enemiesInRangeToAOE.push_back(enemyComponent->GetParent());
							}
						}
					}
				}

				if (enemiesInRangeToAOE.size() > 0)
				{
					for each (std::shared_ptr<Entity> ene in enemiesInRangeToAOE)
					{
						ene->GetComponent<EnemyComponent>()->health -= player->playerAOEAttackDamage / 2.0f;
						ene->GetComponent<EnemyComponent>()->hit = true;
					}
					//player->spinAttackAudio->AudioFile->Play(player->spinAttackAudio->Volume*AudioSystem::VOLUME, player->spinAttackAudio->Pitch, player->spinAttackAudio->Pan);
				}
				enemiesInRangeToAOE.clear();

				player->enemyClicked = false;
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

		if (gettingWeaponCorutine.active)
		{
			if (!(gettingWeaponCorutine.Update()))
			{
				gettingWeapon = false;
				player->navMesh->isMoving = true;
			}
		}
	}
	else
	{
		if (playerRipAttackCorutine.active)
		{
			if (!(playerRipAttackCorutine.UpdateEvent()))
			{
				player->targetedEnemy->GetComponent<EnemyComponent>()->health -= player->playerRipAttackDamage;
				player->targetedEnemy->GetComponent<EnemyComponent>()->hit = true;

				*playerHealth -= player->playerRipPlayerDamage;
				PlayerHit();

				player->ripAttackAudio->AudioFile->Play(player->ripAttackAudio->Volume*AudioSystem::VOLUME, player->ripAttackAudio->Pitch, player->ripAttackAudio->Pan);
			}

			if (!(playerRipAttackCorutine.Update()))
			{
				player->isRipAttack = false;

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->vampireAbility = 0;
				turnOffVampireMode = true;
			}
		}

		if (playerAOEAttackCorutine.active)
		{
			if (!(playerAOEAttackCorutine.Update()))
			{
				player->isAOEAttack = false;

				for each (std::shared_ptr<Entity> ene in enemiesInRangeToAOE)
				{
					ene->GetComponent<EnemyComponent>()->health -= player->playerAOEAttackDamage;
					ene->GetComponent<EnemyComponent>()->hit = true;
				}

				*playerHealth -= player->playerAOEDamage;
				PlayerHit();

				player->vampireAbility = 0;
				enemiesInRangeToAOE.clear();

				turnOffVampireMode = true;
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
		else if (player->isAOEAttack)
		{
			playerRenderableComponent->_modelSkinned->currentAnimation = "4th";
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
		else if (gettingWeapon)
		{
			playerRenderableComponent->_modelSkinned->currentAnimation = "TheSword";
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
		else if ((!player->isWalking) && (player->isSpinAttack))
		{
			player->navMesh->Move(Coroutine::elapsedTime);
			playerRenderableComponent->_modelSkinned->currentAnimation = "SpinAttack";
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
		playerSpinAttackCorutine.active = false;
		enemiesInRangeToAOE.clear();
		player->isNormalAttack = false;
		player->isPowerAttack = false;
		player->isBiteAttack = false;
		player->isSpinAttack = false;
		player->isWalking = false;
		player->attackType = 0;
		player->navMesh->isMoving = false;

		gettingWeaponCorutine.active = false;
		gettingWeapon = false;
	}
	else
	{
		player->vampireAbility = 0;
		playerRipAttackCorutine.active = false;
		playerAOEAttackCorutine.active = false;
		enemiesInRangeToAOE.clear();
		player->isRipAttack = false;
		player->isAOEAttack = false;
		playerRenderableComponent->_modelSkinned->currentAnimation = "Idle";
		player->enemyClicked = false;
		player->targetedEnemy = nullptr;
	}
}