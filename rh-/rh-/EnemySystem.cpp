#include "EnemySystem.h"

EnemySystem::EnemySystem()
{
}

EnemySystem::~EnemySystem()
{
}

void EnemySystem::Iterate()
{
	//for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
	//{
	//	if (!enemyComponent->attackCorutine.active)
	//	{
	//		if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->followPlayerDistance, .1f, enemyComponent->followPlayerDistance)))
	//		{
	//			enemyComponent->navMesh->isMoving = false;
	//			enemyComponent->walking = false;
	//			enemyComponent->attack = false;
	//		}
	//		else if (!XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->attackLength, .1f, enemyComponent->attackLength)))
	//		{
	//			enemyComponent->navMesh->SetDestination(player->GetTransform()->GetPosition());
	//			enemyComponent->navMesh->Move();
	//			enemyComponent->walking = true;
	//			enemyComponent->attack = false;
	//		}
	//		else
	//		{
	//			enemyComponent->navMesh->isMoving = false;
	//			enemyComponent->walking = false;
	//			enemyComponent->attack = true;
	//			enemyComponent->attackCorutine.Restart(enemyComponent->attackLength);
	//		}
	//	}


	//	if (enemyComponent->attackCorutine.active)
	//	{
	//		if (!(enemyComponent->attackCorutine.Update(elapsedTime)))
	//		{
	//			enemyComponent->attack = false;

	//			if (XMVector3NearEqual(enemyComponent->GetParent()->GetTransform()->GetPosition(), player->GetTransform()->GetPosition(), DirectX::SimpleMath::Vector3(enemyComponent->attackLength + 0.5f, .1f, enemyComponent->attackLength + 0.5f)))
	//			{
	//				//playerHealth -= 10.0f;	-- deal Damage 
	//				int a = 5;
	//			}
	//		}
	//	}
	//}
}

void EnemySystem::Initialize()
{
	//IDK
}

void EnemySystem::MyInitialization(std::shared_ptr<Entity> Player, std::shared_ptr<Terrain> Terrain)
{
	for (auto enemyComponent : _world->GetComponents<EnemyComponent>())
	{
		enemyComponent->navMesh = std::make_shared<NavMesh>(Player->GetTransform());
		enemyComponent->navMesh->terrain = Terrain;
		enemyComponent->navMesh->speed = enemyComponent->speed;
	}
}

void EnemySystem::setElapsedTime(float Time)
{
	elapsedTime = Time;
}

void EnemySystem::setPlayer(std::shared_ptr<Entity> Player)
{
	player = Player;
}
