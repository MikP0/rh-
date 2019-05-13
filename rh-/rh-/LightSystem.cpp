#include "LightSystem.h"

LightSystem::LightSystem(std::shared_ptr<EntityManager> entityManager, std::shared_ptr<ToonFactory> fxFactory) : System(entityManager)
{
	_componentsType = ComponentType("Light");

	_fxFactory = fxFactory;
}

LightSystem::~LightSystem()
{
}

void LightSystem::Iterate()
{
	for (auto lightComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<LightComponent>(lightComponent))
		{
			if ((std::dynamic_pointer_cast<LightComponent>(lightComponent))->UpdateAble)
			{
				if ((std::dynamic_pointer_cast<LightComponent>(lightComponent))->LightType == LightTypeComponent::POINTLight)
				{
					_fxFactory->UpdatePointLight((std::dynamic_pointer_cast<LightComponent>(lightComponent))->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
				}
				else if ((std::dynamic_pointer_cast<LightComponent>(lightComponent))->LightType == LightTypeComponent::SPOTLight)
				{
					_fxFactory->UpdateSpotLight((std::dynamic_pointer_cast<LightComponent>(lightComponent))->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
				}
			}
		}
	}
}

void LightSystem::Initialize()
{
	for (auto lightComponent : _entityManager->GetComponents(_componentsType))
	{
		if (std::dynamic_pointer_cast<LightComponent>(lightComponent))
		{
			if ((std::dynamic_pointer_cast<LightComponent>(lightComponent))->LightType == LightTypeComponent::POINTLight)
			{
				_fxFactory->AddPointLight((std::dynamic_pointer_cast<LightComponent>(lightComponent))->Color, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->Position, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->Radius);
			}
			else if ((std::dynamic_pointer_cast<LightComponent>(lightComponent))->LightType == LightTypeComponent::DIRECTLight)
			{
				_fxFactory->AddDirectLight((std::dynamic_pointer_cast<LightComponent>(lightComponent))->Color, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->Direction);
			}
			else if ((std::dynamic_pointer_cast<LightComponent>(lightComponent))->LightType == LightTypeComponent::SPOTLight)
			{
				_fxFactory->AddSpotLight((std::dynamic_pointer_cast<LightComponent>(lightComponent))->Color, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->Direction, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->OuterAngle, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->Position, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->InnerAngle, (std::dynamic_pointer_cast<LightComponent>(lightComponent))->Radius);
			}
		}
	}
}