#include "Include\\Common.fxh"

#define MAX_NUMBER_OF_LIGHT 100

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

	float4 IsNormalMap;
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
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;
};

//------------------------------------------------------------------------------
// Simple shader to do vertex processing on the GPU.
//------------------------------------------------------------------------------
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput OUT = (PixelShaderInput)0;

	float4 pos = float4(input.pos, 1.0f);
	OUT.pos = mul(pos, WorldViewProjection);
	OUT.worldPosition = mul(pos, World).xyz;
	OUT.texCoord = input.texCoord;
	OUT.normal = normalize(mul(float4(input.normal, 0), World).xyz);

	return OUT;
}