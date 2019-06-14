#include "PlayerSystem.h"

PlayerSystem::PlayerSystem(std::shared_ptr<PhysicsSystem> collSys, Camera* cam)
{
	player = nullptr;
	playerHealth = nullptr;

	vampireMode = false;

	playerHealthOrigin = 0;


	playerAttackCorutine.active = false;
	playerBiteCorutine.active = false;
	playerHittedCorutine.active = false;
	playerHealedCorutine.active = false;

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

void PlayerSystem::AdditionalInitialization(std::shared_ptr<Terrain> Terrain)
{
	player->navMesh = std::make_shared<NavMesh>(player->GetParent()->GetTransform());
	player->navMesh->terrain = Terrain;
	player->navMesh->speed = player->playerSpeed;
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
					player->attackType = 1;
					player->enemyClicked = true;
					player->targetedEnemy = coll->OriginObject;
				}
				else
				{
					player->enemyClicked = false;
					player->targetedEnemy = nullptr;
					playerAttackCorutine.active = false;
					playerBiteCorutine.active = false;
					player->isNormalAttack = false;
					player->attackType = 0;
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
				playerAttackCorutine.active = false;
				playerBiteCorutine.active = false;
				player->isNormalAttack = false;
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
					player->attackType = 5;
					player->enemyClicked = true;
					player->targetedEnemy = coll->OriginObject;
				}
				else
				{
					player->enemyClicked = false;
					player->targetedEnemy = nullptr;
					playerAttackCorutine.active = false;
					playerBiteCorutine.active = false;
					player->isNormalAttack = false;
					player->isBiteAttack = false;
					player->attackType = 0;
				}
			}

			///////////////////////

			if ((!player->enemyClicked) || (collisionsWithRay.size() == 0))
			{
				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				playerAttackCorutine.active = false;
				playerBiteCorutine.active = false;
				player->isNormalAttack = false;
				player->isBiteAttack = false;
				player->attackType = 0;
			}
		}
	}


	if (player->enemyClicked)
	{
		if ((!playerAttackCorutine.active) && (!playerBiteCorutine.active))
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
					playerAttackCorutine.Restart(1.5f);
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
	if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D1))
	{
		player->vampireAbility = 1;
	}

	if (player->vampireAbility == 1)
	{
		if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
		{
			Vector3 destination = Raycast::GetPointOnGround(*camera);
			playerEntity->GetTransform()->SetPosition(destination);
			player->vampireAbility = 0;
			*playerHealth -= 15.0f;

			playerRenderableComponent->_modelSkinned->isHitted = true;
			playerHittedCorutine.Restart(0.1f);
		}
	}

	
	if (keyboardTracker.IsKeyPressed(Keyboard::Keys::D2))
	{
		player->vampireAbility = 2;
	}

	if (player->vampireAbility == 2)
	{
		if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
		{
			Vector3 destination = Raycast::GetPointOnGround(*camera);
			playerEntity->GetTransform()->SetPosition(destination);
			player->vampireAbility = 0;
			*playerHealth -= 15.0f;

			playerRenderableComponent->_modelSkinned->isHitted = true;
			playerHittedCorutine.Restart(0.1f);


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


		}
	}
}

void PlayerSystem::UpdateCorutines()
{
	if (!vampireMode)
	{
		if (playerAttackCorutine.active)
		{
			if (!(playerAttackCorutine.Update()))
			{
				player->isNormalAttack = false;

				if (XMVector3NearEqual(playerEntity->GetTransform()->GetPosition(), player->targetedEnemy->GetTransform()->GetPosition(), Vector3(1.5f, .1f, 1.5f)))
				{
					player->targetedEnemy->GetComponent<EnemyComponent>()->health -= 15.0f;
				}

				player->hittedEnemy = player->targetedEnemy;

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->attackType = 0;


				player->hittedEnemy->GetComponent<RenderableComponent>()->_modelSkinned->isHitted = true;
				enemyHittedCorutine.Restart(0.1f);
			}
		}

		if (playerBiteCorutine.active)
		{
			if (!(playerBiteCorutine.Update()))
			{
				player->isBiteAttack = false;

				player->targetedEnemy->GetComponent<EnemyComponent>()->health -= 20.0f;

				*playerHealth += 15.0f;

				if (*playerHealth > playerHealthOrigin)
					* playerHealth = playerHealthOrigin;

				playerRenderableComponent->_modelSkinned->isHealed = true;
				playerHealedCorutine.Restart(0.1f);


				player->targetedEnemy->GetComponent<EnemyComponent>()->bited = false;

				player->hittedEnemy = player->targetedEnemy;

				player->enemyClicked = false;
				player->targetedEnemy = nullptr;
				player->attackType = 0;

				player->hittedEnemy->GetComponent<RenderableComponent>()->_modelSkinned->isHitted = true;
				enemyHittedCorutine.Restart(0.1f);
			}
		}
	}


	if (enemyHittedCorutine.active)
	{
		if (!(enemyHittedCorutine.Update()))
		{
			player->hittedEnemy->GetComponent<RenderableComponent>()->_modelSkinned->isHitted = false;
			player->hittedEnemy = nullptr;
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
		playerRenderableComponent->_modelSkinned->currentAnimation = "Idle";
	}
	else
	{
		if (player->isWalking)
		{
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
		playerAttackCorutine.active = false;
		playerBiteCorutine.active = false;
		player->isNormalAttack = false;
		player->isBiteAttack = false;
		player->isWalking = false;
		player->attackType = 0;
		player->navMesh->isMoving = false;
	}
	else
	{
		player->vampireAbility = 0;
	}
}