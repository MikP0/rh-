#include "EnemySystem.h"

EnemySystem::EnemySystem()
{
}

EnemySystem::~EnemySystem()
{
}

void EnemySystem::Iterate()
{
	for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
	{
		// state

		if (enemyComponent->bited)
		{
			enemyComponent->navMesh->isMoving = false;
			enemyComponent->walking = false;
			enemyComponent->attack = false;
			enemyComponent->attackCorutine.active = false;
		}
		else
		{
			if (!enemyComponent->attackCorutine.active)
			{
				if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance, .1f, enemyComponent->followPlayerDistance)))
				{
					enemyComponent->navMesh->isMoving = false;
					enemyComponent->walking = false;
					enemyComponent->attack = false;
				}
				else if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->distanceToAttack, .1f, enemyComponent->distanceToAttack)))
				{
					enemyComponent->navMesh->SetDestination(player->GetTransform()->GetPosition());
					enemyComponent->navMesh->Move();
					enemyComponent->walking = true;
					enemyComponent->attack = false;
				}
				else
				{
					enemyComponent->navMesh->isMoving = false;
					enemyComponent->walking = false;
					enemyComponent->attack = true;
					enemyComponent->attackCorutine.Restart(enemyComponent->attackLength);
				}
			}


			if (enemyComponent->attackCorutine.active)
			{
				if (!(enemyComponent->attackCorutine.Update()))
				{
					enemyComponent->attack = false;

					if (XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->attackLength + 0.5f, .1f, enemyComponent->attackLength + 0.5f)))
					{
						*playerHealth -= enemyComponent->damage;
					}
				}
			}
		}

		// animations and position
		if (enemyComponent->bited)
		{
			enemyComponent->GetParent()->GetComponent<RenderableComponent>()->_modelSkinned->playingAnimation = false;
		}
		else if (enemyComponent->walking)
		{
			enemyComponent->GetParent()->GetComponent<RenderableComponent>()->_modelSkinned->SetCurrentAnimation("Walk");
		}
		else if ((!enemyComponent->walking) && (enemyComponent->attack))
		{
			float dot = enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).x + enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).z;
			float cross = enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).z - enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			enemyComponent->GetParent()->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			enemyComponent->GetParent()->GetComponent<RenderableComponent>()->_modelSkinned->SetCurrentAnimation("Attack");
		}
		else if ((!enemyComponent->walking) && (!enemyComponent->attack))
		{
			float dot = enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).x + enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).z;
			float cross = enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().x * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).z - enemyComponent->GetParent()->GetTransform()->GetTransformMatrix().Forward().z * (player->GetTransform()->GetPosition() - enemyComponent->GetParent()->GetTransform()->GetPosition()).x;
			float fAngle = (atan2(cross, dot) * 180.0f / 3.14159f) + 180.0f;
			enemyComponent->GetParent()->GetTransform()->Rotate(dxmath::Vector3(0, 1, 0), XMConvertToRadians(-fAngle));

			enemyComponent->GetParent()->GetComponent<RenderableComponent>()->_modelSkinned->SetCurrentAnimation("Idle");
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
	}
}