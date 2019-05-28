#pragma once

#include "pch.h"
#include "Component.h"

enum LightTypeComponent
{
	POINTLight = 1,
	DIRECTLight = 2,
	SPOTLight = 3
};

class LightComponent : public Component
{
public:

	//Methods
	LightComponent(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction);
	LightComponent(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Position, float Radius, bool updateAble = false);
	LightComponent(DirectX::XMFLOAT4 Color, DirectX::XMFLOAT3 Direction, float OuterAngle, DirectX::XMFLOAT3 Position, float InnerAngle, float Radius, bool updateAble = false);
	

	virtual ~LightComponent();

	//Fields
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 Direction;
	float OuterAngle;
	DirectX::XMFLOAT3 Position;
	float InnerAngle;
	float Radius;

	bool UpdateAble;

	LightTypeComponent LightType;

	int Id;

	static int nextPLId;
	static int nextSLId;
	static int nextDLId;
};

