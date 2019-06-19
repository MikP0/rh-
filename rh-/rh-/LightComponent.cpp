#include "pch.h"
#include "LightComponent.h"

int LightComponent::nextDLId = 0;
int LightComponent::nextPLId = 0;
int LightComponent::nextSLId = 0;

LightComponent::LightComponent(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction)
{
	this->Color = Color;
	this->Direction = Direction;
	this->LightType = LightTypeComponent::DIRECTLight;
	this->UpdateAble = false;
	this->Id = nextDLId++;
}

LightComponent::LightComponent(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Position, float Radius, bool updateAble)
{
	this->Color = Color;
	this->Position = Position;
	this->Radius = Radius;
	this->LightType = LightTypeComponent::POINTLight;
	this->UpdateAble = updateAble;
	this->Id = nextPLId++;
}

LightComponent::LightComponent(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction, float OuterAngle, DirectX::XMFLOAT3 Position, float InnerAngle, float Radius, bool updateAble)
{
	this->Color = Color;
	this->Direction = Direction;
	this->OuterAngle = OuterAngle;
	this->Position = Position;
	this->InnerAngle = InnerAngle;
	this->Radius = Radius;
	this->LightType = LightTypeComponent::SPOTLight;
	this->UpdateAble = updateAble;
	this->Id = nextSLId++;
}

LightComponent::~LightComponent()
{
}
