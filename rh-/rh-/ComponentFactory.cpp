#include "pch.h"
#include "ComponentFactory.h"

ComponentFactory::ComponentFactory()
{
	_entityManager = std::make_shared<EntityManager>();
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
