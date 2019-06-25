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

//------------------------------------------------------------------------------
// Per-vertex data used as input to the vertex shader.
//------------------------------------------------------------------------------
struct VertexShaderInput
{
	float3 pos : SV_Position;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
};

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

//------------------------------------------------------------------------------
// Simple shader to do vertex processing on the GPU.
//------------------------------------------------------------------------------
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput OUT = (PixelShaderInput)0;

	OUT.PosW = mul(float4(input.pos, 1.0f), World).xyz;
	OUT.NormalW = mul(input.normal, (float3x3)WorldInverseTranspose);
	OUT.PosH = mul(float4(input.pos, 1.0f), WorldViewProjection);
	OUT.Tex = input.texCoord;

	return OUT;
}