#include "Include\\Common.fxh"

#define MAX_NUMBER_OF_LIGHT 25

cbuffer StaticBuffer : register(b0)
{
	//For Tex and Mat
	float4 DiffuseColor;
	float3 EmissiveColor;
	//For lights
	float SpecularPower;
	float4 AmbientColor;
	float4 SpecularColor;

	//Lights
	POINT_LIGHT PointLight[MAX_NUMBER_OF_LIGHT];
	DIRECTIONAL_LIGHT DirectionalLight[MAX_NUMBER_OF_LIGHT];
	SPOT_LIGHT SpotLight[MAX_NUMBER_OF_LIGHT];

	float4 NumOfLights;	//x - point, y - dir, z - spot
};

//------------------------------------------------------------------------------
cbuffer DynamicBuffer : register(b1)
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldViewProjection;
	float3 CameraPosition;
	float IsTextured;

	float4x4 WorldInverseTranspose;
};

////float4 TintColor = float4(1, 1, 1, 1);

//------------------------------------------------------------------------------
// Per-pixel color data passed through the pixel shader.
//------------------------------------------------------------------------------
struct PixelShaderInput
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
};

SamplerState ColorSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};



//------------------------------------------------------------------------------
// Textures and Samplers
//------------------------------------------------------------------------------
//Texture2D ColorMap : register(t0);

TextureCube<float4> SkyboxTexture : register(t0);

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

//------------------------------------------------------------------------------
// A pass-through function for the (interpolated) color data.
//------------------------------------------------------------------------------
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 OUT = (float4)0;
	

	float3 NormalW = normalize(input.NormalW);

	float3 toEye = CameraPosition - input.PosW;

	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;


	//float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 litColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float3 incident = -toEye;
	float3 reflectionVector = reflect(incident, NormalW);
	float4 reflectionColor = SkyboxTexture.Sample(samAnisotropic, reflectionVector);

	litColor = reflectionColor;






	OUT.rgb = litColor;
	OUT.a = 1.0f;





	return OUT;
}





/*
float4 OUT = (float4)0;


float3 NormalW = normalize(input.NormalW);

float3 toEye = CameraPosition - input.PosW;

float distToEye = length(toEye);

// Normalize.
toEye /= distToEye;


//float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

float4 litColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

float3 incident = -toEye;
float3 reflectionVector = reflect(incident, NormalW);
float4 reflectionColor = SkyboxTexture.Sample(samAnisotropic, reflectionVector);

litColor = reflectionColor;


OUT = litColor;

return OUT;*/