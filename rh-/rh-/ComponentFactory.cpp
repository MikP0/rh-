#include "pch.h"
#include "ComponentFactory.h"

ComponentFactory::ComponentFactory(std::shared_ptr<EntityManager> entityManager)
{
	_entityManager = std::move(entityManager);
}

ComponentFactory::~ComponentFactory()
{
}

void ComponentFactory::CreateComponent(int entityId, std::shared_ptr<Component> component)
{
	_entityManager->AddComponent(entityId, component);
}

void ComponentFactory::CreateComponent(std::shared_ptr<Entity> entity, std::shared_ptr<Component> component)
{
	_entityManager->AddComponent(entity, component);
}
