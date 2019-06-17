#include "EnemySystem.h"

EnemySystem::EnemySystem()
{
	vampireMode = false;
}

EnemySystem::~EnemySystem()
{
}

void EnemySystem::Iterate()
{
	if (!vampireMode)
	{
		for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
		{
			if (enemyComponent->_isEnabled)
			{
				if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance + 5.0f, .1f, enemyComponent->followPlayerDistance + 5.0f)))
				{
					enemyComponent->enemyState = EnemyState::IDLE;
					enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = false;
					enemyComponent->navMesh->isMoving = false;
				}
				else
				{
					SetStates(enemyComponent);
					ApplyStates(enemyComponent);
					CheckCorutines(enemyComponent);
				}
			}
		}
	}
	else
	{
		for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
		{
			if (enemyComponent->_isEnabled)
			{
				enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = false;
			}
		}
	}
}


void EnemySystem::SetStates(std::shared_ptr<EnemyComponent> enemy)
{
	if (enemy->health <= 0)
	{
		enemy->enemyState = EnemyState::DYING;

		if (!enemy->dyingCorutine.active)
		{
			enemy->dying = true;

			enemy->attackCorutine.active = false;
			enemy->navMesh->isMoving = false;

			enemy->enemyRenderableComponent->_modelSkinned->isHitted = false;

			enemy->dyingCorutine.Restart(2.51f);
		}
	}
	else if (enemy->bited)
	{
		enemy->enemyState = EnemyState::BITED;

		enemy->attackCorutine.active = false;
		enemy->navMesh->isMoving = false;

		enemy->enemyRenderableComponent->_modelSkinned->isHitted = false;
	}
	else if ((!enemy->dyingCorutine.active) && (!enemy->attackCorutine.active) && (!enemy->hitCorutine.active))
	{
		if (enemy->hit)
		{
			enemy->enemyState = EnemyState::HIT;

			enemy->enemyRenderableComponent->_modelSkinned->isHitted = true;
			enemy->hitCorutine.Restart(0.5f);
			enemy->hitColorCorutine.Restart(0.1f);
		}
		else if (!XMVector3NearEqual(enemy->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemy->followPlayerDistance, .1f, enemy->followPlayerDistance)))
		{
			enemy->enemyState = EnemyState::IDLE;

			enemy->navMesh->isMoving = false;
		}
		else if (!XMVector3NearEqual(enemy->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemy->distanceToAttack, .1f, enemy->distanceToAttack)))
		{
			enemy->enemyState = EnemyState::FOLLOW;

			enemy->navMesh->SetEnemyDestination(player->GetTransform()->GetPosition());
			enemy->navMesh->Move(Coroutine::elapsedTime);

			if (enemy->footstepAudio != nullptr) {
				enemy->footstepAudio->Mute = false;
				enemy->footstepAudio->Volume = 0.000000000001f;
				if (enemy->footstepAudio->AudioLoopInstance->GetState() != SoundState::PLAYING) {
					enemy->footstepAudio->AudioFile->Play();
				}
			}
		}
		else
		{
			enemy->enemyState = EnemyState::ATTACK;

			float dot = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).x + enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).z;
			float cross = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).z - enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			enemy->GetParent()->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			enemy->navMesh->isMoving = false;

			enemy->attackCorutine.Restart(enemy->attackLength);
		}
	}
}

void EnemySystem::ApplyStates(std::shared_ptr<EnemyComponent> enemy)
{
	if (enemy->enemyState == EnemyState::DYING)
	{
		enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Dying");
	}
	else if (enemy->enemyState == EnemyState::BITED)
	{
		enemy->enemyRenderableComponent->_modelSkinned->playingAnimation = false;
	}
	else if (enemy->enemyState == EnemyState::HIT)
	{
		enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Hit");
	}
	else if (enemy->enemyState == EnemyState::FOLLOW)
	{
		enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Walk");
	}
	else if (enemy->enemyState == EnemyState::IDLE)
	{
		enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
	}
	else if (enemy->enemyState == EnemyState::ATTACK)
	{
		enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Attack");
	}


	/*if (enemy->enemyState == EnemyState::DYING)
	{
		if (!enemy->dyingCorutine.active)
		{
			enemy->dying = true;

			enemy->attackCorutine.active = false;
			enemy->navMesh->isMoving = false;

			enemy->enemyRenderableComponent->_modelSkinned->isHitted = false;
			enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Dying");

			enemy->dyingCorutine.Restart(2.51f);
		}
	}
	else if (enemy->enemyState == EnemyState::BITED)
	{
		enemy->attackCorutine.active = false;
		enemy->navMesh->isMoving = false;

		enemy->enemyRenderableComponent->_modelSkinned->isHitted = false;
		enemy->enemyRenderableComponent->_modelSkinned->playingAnimation = false;
	}
	else if ((!enemy->dyingCorutine.active) && (!enemy->attackCorutine.active) && (!enemy->hitCorutine.active))
	{
		if (enemy->enemyState == EnemyState::HIT)
		{
			enemy->enemyRenderableComponent->_modelSkinned->isHitted = true;
			enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Hit");
			enemy->hitCorutine.Restart(0.5f);
			enemy->hitColorCorutine.Restart(0.1f);
		}
		else if (enemy->enemyState == EnemyState::FOLLOW)
		{
			enemy->navMesh->SetEnemyDestination(player->GetTransform()->GetPosition());
			enemy->navMesh->Move(Coroutine::elapsedTime);

			enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Walk");
		}
		else if (enemy->enemyState == EnemyState::IDLE)
		{
			enemy->navMesh->isMoving = false;

			enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
		}
		else if (enemy->enemyState == EnemyState::ATTACK)
		{
			float dot = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).x + enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).z;
			float cross = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).z - enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			enemy->GetParent()->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			enemy->navMesh->isMoving = false;

			enemy->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Attack");

			enemy->attackCorutine.Restart(enemy->attackLength);
		}
	}*/
}

void EnemySystem::CheckCorutines(std::shared_ptr<EnemyComponent> enemy)
{
	if (enemy->dyingCorutine.active)
	{
		if (!(enemy->dyingCorutine.Update()))
		{
			enemy->enemyState = EnemyState::DEAD;

			enemy->enemyRenderableComponent->_modelSkinned->playingAnimation = false;

			enemy->enemyRenderableComponent->SetIsEnabled(false);
			enemy->GetParent()->GetComponent<PhysicsComponent>()->SetIsEnabled(false);
			enemy->SetIsEnabled(false);
			enemy->GetParent()->SetActive(false);

			player->GetComponent<PlayerComponent>()->AddPlayerHealth(1.0f);
		}
	}

	if (enemy->attackCorutine.active)
	{
		if (!(enemy->attackCorutine.Update()))
		{
			if (CheckRangeAndCone(enemy, player->GetTransform()->GetPosition(), enemy->attackLength, 60.f))
			{
				*playerHealth -= enemy->damage;

				player->GetComponent<PlayerComponent>()->isHit = true;
			}
		}
	}

	if (enemy->hitCorutine.active)
	{
		if (!(enemy->hitCorutine.Update()))
		{
			enemy->hit = false;
		}
	}

	if (enemy->hitColorCorutine.active)
	{
		if (!(enemy->hitColorCorutine.Update()))
		{
			enemy->enemyRenderableComponent->_modelSkinned->isHitted = false;
		}
	}
}

void EnemySystem::Initialize()
{
	//Nothing
}

void EnemySystem::AdditionalInitialization(std::shared_ptr<Entity> Player, std::shared_ptr<Terrain> Terrain, std::shared_ptr<float> PlayerHealth)
{
	player = Player;
	playerHealth = PlayerHealth;

	for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
	{
		enemyComponent->navMesh = std::make_shared<NavMesh>(enemyComponent->GetParent()->GetTransform());
		enemyComponent->navMesh->terrain = Terrain;
		enemyComponent->navMesh->speed = enemyComponent->speed;
		enemyComponent->enemyRenderableComponent = enemyComponent->GetParent()->GetComponent<RenderableComponent>();
	}
}

void EnemySystem::SetVampireMode(bool mode)
{
	vampireMode = mode;
}

bool EnemySystem::CheckRangeAndCone(std::shared_ptr<EnemyComponent> enemy, DirectX::SimpleMath::Vector3 vecTar, float distance, float Cone)
{
	bool inRange = false;

	if (XMVector3NearEqual(enemy->GetParent()->GetTransform()->GetPosition(), vecTar, DirectX::SimpleMath::Vector3(distance, .1f, distance)))
	{
		float dot = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (vecTar - enemy->GetParent()->GetTransform()->GetPosition()).x + enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (vecTar - enemy->GetParent()->GetTransform()->GetPosition()).z;
		float cross = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (vecTar - enemy->GetParent()->GetTransform()->GetPosition()).z - enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (vecTar - enemy->GetParent()->GetTransform()->GetPosition()).x;
		float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;


		if (((fAngle >= 0) && (fAngle <= Cone)) || ((fAngle <= 360) && (fAngle >= 360 - Cone)))
		{
			inRange = true;
		}
	}

	return inRange;
}