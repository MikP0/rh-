#include "Include\\Common.fxh"

#define NUM_POINT_LIGHTS 3
#define NUM_DIRECT_LIGHTS 0
#define NUM_REF_LIGHTS 1


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
	POINT_LIGHT PointLight[3];
	DIRECTIONAL_LIGHT DirectionalLight[3];
	SPOT_LIGHT SpotLight[3];
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
// Textures and Samplers
//------------------------------------------------------------------------------
Texture2D ColorMap : register(t0);
Texture2D NormalMap : register(t1);

SamplerState ColorSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

//------------------------------------------------------------------------------
// A pass-through function for the (interpolated) color data.
//------------------------------------------------------------------------------
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 OUT = (float4)0;

	float3 normal = normalize(input.normal);
	float3 viewDirection = normalize(CameraPosition - input.worldPosition);

	float4 color;

	if (IsTextured == 0.0f)
	{
		color = DiffuseColor;
	}
	else
	{
		color = ColorMap.Sample(ColorSampler, input.texCoord);
	}


	// POINT LIGHTS
	LIGHT_CONTRIBUTION_DATA lightContributionData;
	lightContributionData.Color = color;
	lightContributionData.Normal = normal;
	lightContributionData.ViewDirection = viewDirection;
	lightContributionData.SpecularColor = SpecularColor;
	lightContributionData.SpecularPower = SpecularPower;

	float3 totalLightContribution = (float3)0;

	[unroll]
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		lightContributionData.LightDirection = get_light_data(PointLight[i].Position, input.worldPosition, PointLight[i].Radius);
		lightContributionData.LightColor = PointLight[i].Color;
		totalLightContribution += get_light_contribution(lightContributionData);
	}


	// DIRECT LIGHTS
	float3 lightDirection;
	float n_dot_l;
	float3 diffuse;

	for (int i = 0; i < NUM_DIRECT_LIGHTS; i++)
	{
		lightDirection = normalize(-DirectionalLight[i].Direction);
		n_dot_l = dot(lightDirection, normal);
		diffuse = (float3)0;
		if (n_dot_l > 0)
		{
			diffuse = DirectionalLight[i].Color.rgb * DirectionalLight[i].Color.a * n_dot_l * color.rgb;
		}

		totalLightContribution += diffuse;
	}

	// SPOT LIGHTS
	float3 result;
	for (int i = 0; i < NUM_REF_LIGHTS; i++)
	{
		result = get_spot_light(SpotLight[i], color, normal, input.worldPosition, viewDirection, SpecularColor, SpecularPower);
		totalLightContribution += result;
	}

	float3 ambient = get_vector_color_contribution(AmbientColor, color.rgb);

	OUT.rgb = ambient + totalLightContribution;
	OUT.a = 1.0f;


	//Lights off
	//OUT.rgb = color.rgb;
	//OUT.a = 1.0f;

	return OUT;
}