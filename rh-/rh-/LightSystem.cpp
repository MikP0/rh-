#include "LightSystem.h"

LightSystem::LightSystem(std::shared_ptr<ShadowFactory> ShadowsfxFactory, std::shared_ptr<ToonFactory> NoShadowsfxFactory)
{
	_ShadowsfxFactory = ShadowsfxFactory;
	_noShadowsfxFactory = NoShadowsfxFactory;
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
				_ShadowsfxFactory->UpdatePointLight(lightComponent->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
				_noShadowsfxFactory->UpdatePointLight(lightComponent->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
			}
			else if (lightComponent->LightType == LightTypeComponent::SPOTLight)
			{
				_ShadowsfxFactory->UpdateSpotLight(lightComponent->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
				_noShadowsfxFactory->UpdateSpotLight(lightComponent->Id, lightComponent->GetParent()->GetTransform()->GetPosition());
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
			_ShadowsfxFactory->AddPointLight(lightComponent->Color, lightComponent->Position, lightComponent->Radius);
			_noShadowsfxFactory->AddPointLight(lightComponent->Color, lightComponent->Position, lightComponent->Radius);
		}
		else if (lightComponent->LightType == LightTypeComponent::DIRECTLight)
		{
			_ShadowsfxFactory->AddDirectLight(lightComponent->Color, lightComponent->Direction);
			_noShadowsfxFactory->AddDirectLight(lightComponent->Color, lightComponent->Direction);
		}
		else if (lightComponent->LightType == LightTypeComponent::SPOTLight)
		{
			_ShadowsfxFactory->AddSpotLight(lightComponent->Color, lightComponent->Direction, lightComponent->OuterAngle, lightComponent->Position, lightComponent->InnerAngle, lightComponent->Radius);
			_noShadowsfxFactory->AddSpotLight(lightComponent->Color, lightComponent->Direction, lightComponent->OuterAngle, lightComponent->Position, lightComponent->InnerAngle, lightComponent->Radius);
		}
	}
}