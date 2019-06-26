#include "pch.h"
#include "EnemySystem.h"
#include "AudioSystem.h"

EnemySystem::EnemySystem()
{
	vampireMode = false;
	menuIsOn = false;
	stopInput = false;
	bossModeCheck = false;
	boosAttackCorutine.active = false;
	boosIdleCorutine.active = false;

	endGame = false;
}

EnemySystem::~EnemySystem()
{
}

void EnemySystem::Iterate()
{
	if ((!menuIsOn) && (!stopInput))
	{
		if ((!vampireMode) && (!humanMode))
		{
			for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
			{
				if (enemyComponent->isBoss)
				{	
					enemyComponent->enemyState = EnemyState::IDLE;
					enemyComponent->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
					enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = true;
				}
				else if (enemyComponent->_isEnabled)
				{
					if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance + 15.0f, .1f, enemyComponent->followPlayerDistance + 15.0f)))
					{
						enemyComponent->enemyState = EnemyState::IDLE;
						enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = false;
						enemyComponent->navMesh->isMoving = false;
					}
					else if(!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance + 5.0f, .1f, enemyComponent->followPlayerDistance + 5.0f)))
					{
						enemyComponent->enemyState = EnemyState::IDLE;
						enemyComponent->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
						enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = true;
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
		else if (humanMode)
		{
			for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
			{
				if (enemyComponent->_isEnabled)
				{
					if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance + 15.0f, .1f, enemyComponent->followPlayerDistance + 15.0f)))
					{
						enemyComponent->enemyState = EnemyState::IDLE;
						enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = false;
						enemyComponent->navMesh->isMoving = false;
					}
					else if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance + 5.0f, .1f, enemyComponent->followPlayerDistance + 5.0f)))
					{
						enemyComponent->enemyState = EnemyState::IDLE;
						enemyComponent->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
						enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = true;
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
	else if (bossModeCheck)
	{
		for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
		{
			if (enemyComponent->isBoss)
			{
				BossMode(enemyComponent);	
			}
			else
			{
				if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance + 15.0f, .1f, enemyComponent->followPlayerDistance + 15.0f)))
				{
					enemyComponent->enemyState = EnemyState::IDLE;
					enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = false;
					enemyComponent->navMesh->isMoving = false;
				}
				else
				{
					enemyComponent->enemyState = EnemyState::IDLE;
					enemyComponent->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
					enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = true;
				}
			}
		}
	}
}


void EnemySystem::SetStates(std::shared_ptr<EnemyComponent> enemy)
{
	if (enemy->health <= 0)
	{
		enemy->enemyState = EnemyState::DYING;
		enemy->navMesh->currentPath.clear();
		if (!enemy->dyingCorutine.active)
		{
			enemy->dying = true;

			enemy->attackCorutine.active = false;
			enemy->navMesh->isMoving = false;

			enemy->enemyRenderableComponent->_modelSkinned->isHitted = false;

			enemy->dyingCorutine.Restart(2.51f);

			enemy->enemyRenderableComponent->_modelSkinned->isHitted = true;
			enemy->hitColorCorutine.Restart(0.1f);

			enemy->deathAudio->AudioFile->Play(enemy->deathAudio->Volume*AudioSystem::VOLUME, enemy->deathAudio->Pitch, enemy->deathAudio->Pan);

		}
	}
	else if (enemy->bited)
	{
		enemy->enemyState = EnemyState::BITED;
		enemy->navMesh->currentPath.clear();
		enemy->attackCorutine.active = false;
		enemy->navMesh->isMoving = false;

		enemy->enemyRenderableComponent->_modelSkinned->isHitted = false;
	}
	else if ((!enemy->dyingCorutine.active) && (!enemy->hitCorutine.active))
	{
		if (enemy->hit)
		{
			enemy->enemyState = EnemyState::HIT;
			enemy->navMesh->currentPath.clear();
			enemy->enemyRenderableComponent->_modelSkinned->isHitted = true;
			enemy->hitCorutine.Restart(0.5f);
			enemy->hitColorCorutine.Restart(0.1f);

			enemy->attackCorutine.active = false;

			enemy->damageAudio->AudioFile->Play(enemy->damageAudio->Volume*AudioSystem::VOLUME, enemy->damageAudio->Pitch, enemy->damageAudio->Pan);
		}
		else if (!enemy->attackCorutine.active)
		{
			if (!XMVector3NearEqual(enemy->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemy->distanceToAttack, .1f, enemy->distanceToAttack)))
			{
				enemy->enemyState = EnemyState::FOLLOW;

				enemy->navMesh->SetEnemyDestination(player->GetTransform()->GetPosition());
				enemy->navMesh->Move(Coroutine::elapsedTime);
				//enemy->navMesh->MoveEnemy(Coroutine::elapsedTime, enemy->GetParent()->GetComponent<PhysicsComponent>());
			}
			else
			{
				enemy->enemyState = EnemyState::ATTACK;
				enemy->navMesh->currentPath.clear();
				float dot = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).x + enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).z;
				float cross = enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).z - enemy->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemy->GetParent()->GetTransform()->GetPosition()).x;
				float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
				enemy->GetParent()->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

				if (!enemy->isGuard)
					enemy->attackCorutine.RestartWithEvent(enemy->attackLength, enemy->attackDamageTime);
				else
					enemy->attackCorutine.RestartWithEvent(enemy->attackLength, enemy->attackDamageTime - 0.1f);

				enemy->normalAttackAudio->AudioFile->Play(enemy->normalAttackAudio->Volume*AudioSystem::VOLUME, enemy->normalAttackAudio->Pitch, enemy->normalAttackAudio->Pan);
			}
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
		enemy->footstepAudio->Mute = false;
		if (enemy->footstepAudio->AudioLoopInstance->GetState() != SoundState::PLAYING) {
			enemy->footstepAudio->AudioFile->Play(enemy->footstepAudio->Volume*AudioSystem::VOLUME, enemy->footstepAudio->Pitch, enemy->footstepAudio->Pan);
		}
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

			player->GetComponent<PlayerComponent>()->isHealed = true;
			player->GetComponent<PlayerComponent>()->AddPlayerHealth(1.0f);
		}
	}

	if (enemy->attackCorutine.active)
	{
		if (!(enemy->attackCorutine.UpdateEvent()))
		{
			if (CheckRangeAndCone(enemy, player->GetTransform()->GetPosition(), enemy->distanceToAttack + 1.5f, 80.f))
			{
				*playerHealth -= enemy->damage;

				player->GetComponent<PlayerComponent>()->isHit = true;
				player->GetComponent<PlayerComponent>()->damageAudio->AudioFile->Play(player->GetComponent<PlayerComponent>()->damageAudio->Volume*AudioSystem::VOLUME, player->GetComponent<PlayerComponent>()->damageAudio->Pitch, player->GetComponent<PlayerComponent>()->damageAudio->Pan);
			}
		}

		if (!(enemy->attackCorutine.Update()))
		{
			// DO NOTHING until animation is done
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

int EnemySystem::RespawnEnemiesFromCheckpoint()
{
	int cp = 4;

	for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
	{
		if (enemyComponent->enemyState != EnemyState::DEAD && enemyComponent->enemyState != EnemyState::DYING) {
			cp = enemyComponent->checkpointNumber;
			break;
		}
	}

	for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
	{
		if (enemyComponent->enemyState != EnemyState::DEAD && enemyComponent->enemyState != EnemyState::DYING) {
			if (cp > enemyComponent->checkpointNumber)
				cp = enemyComponent->checkpointNumber;
		}
	}

	for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
	{
		if (enemyComponent->checkpointNumber >= cp) {
			enemyComponent->GetParent()->GetTransform()->SetPosition(enemyComponent->originPosition);
			enemyComponent->health = enemyComponent->originHealth;

			enemyComponent->enemyState = EnemyState::IDLE;

			enemyComponent->enemyRenderableComponent->_modelSkinned->playingAnimation = true;

			enemyComponent->enemyRenderableComponent->SetIsEnabled(true);
			enemyComponent->GetParent()->GetComponent<PhysicsComponent>()->SetIsEnabled(true);
			enemyComponent->SetIsEnabled(true);
			enemyComponent->GetParent()->SetActive(true);
			enemyComponent->dying = false;
			enemyComponent->bited = false;
			enemyComponent->hit = false;
			enemyComponent->enemyRenderableComponent->_modelSkinned->isHitted = false;
			enemyComponent->hitCorutine.active = false;
			enemyComponent->hitColorCorutine.active = false;

		}
	}
	return cp;
}

void EnemySystem::BossMode(std::shared_ptr<EnemyComponent> boss)
{
	if (!boosAttackCorutine.active)
	{
		boss->enemyState = EnemyState::IDLE;
		boss->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
		boss->enemyRenderableComponent->_modelSkinned->playingAnimation = true;	
	}


	if (boosIdleCorutine.active)
	{
		if (!(boosIdleCorutine.Update()))
		{
			boosIdleCorutine.active = false;
			boss->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Run");
			boss->enemyRenderableComponent->_modelSkinned->playingAnimation = true;
			boosAttackCorutine.Restart(1.0f);

			float dot = boss->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - boss->GetParent()->GetTransform()->GetPosition()).x + boss->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - boss->GetParent()->GetTransform()->GetPosition()).z;
			float cross = boss->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - boss->GetParent()->GetTransform()->GetPosition()).z - boss->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - boss->GetParent()->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			boss->GetParent()->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));
		}
	}

	if (boosAttackCorutine.active)
	{
		boss->footstepAudio->Mute = false;
		if (boss->footstepAudio->AudioLoopInstance->GetState() != SoundState::PLAYING) {
			boss->footstepAudio->AudioFile->Play(boss->footstepAudio->Volume*AudioSystem::VOLUME, boss->footstepAudio->Pitch, boss->footstepAudio->Pan);
		}

		if (!(boosAttackCorutine.Update()))
		{
			boosAttackCorutine.active = false;
			boss->enemyState = EnemyState::IDLE;
			boss->enemyRenderableComponent->_modelSkinned->SetCurrentAnimation("Idle");
			boss->enemyRenderableComponent->_modelSkinned->playingAnimation = true;
			endGame = true;
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
		enemyComponent->originPosition = enemyComponent->GetParent()->GetTransform()->GetPosition();
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