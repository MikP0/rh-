#include "LightSystem.h"

LightSystem::LightSystem(std::shared_ptr<ShadowFactory> fxFactory)
{
	_fxFactory = fxFactory;
}

LightSystem::~LightSystem()
{
}

void LightSystem::Iterate()
{
	for (auto lightComponent : _world->GetComponents<LightComponent>())
	{
		if (lightComponent->UpdateAble)
		{
			if (lightComponent->LightType == LightTypeComponent::POINTLight)
			{
				_fxFactory->UpdatePointLight(lightComponent->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
			}
			else if (lightComponent->LightType == LightTypeComponent::SPOTLight)
			{
				_fxFactory->UpdateSpotLight(lightComponent->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
			}
		}
	}
}

void LightSystem::Initialize()
{
	for (auto lightComponent : _world->GetComponents<LightComponent>())
	{
		if (lightComponent->LightType == LightTypeComponent::POINTLight)
		{
			_fxFactory->AddPointLight(lightComponent->Color, lightComponent->Position, lightComponent->Radius);
		}
		else if (lightComponent->LightType == LightTypeComponent::DIRECTLight)
		{
			_fxFactory->AddDirectLight(lightComponent->Color, lightComponent->Direction);
		}
		else if (lightComponent->LightType == LightTypeComponent::SPOTLight)
		{
			_fxFactory->AddSpotLight(lightComponent->Color, lightComponent->Direction, lightComponent->OuterAngle, lightComponent->Position, lightComponent->InnerAngle, lightComponent->Radius);
		}
	}
}