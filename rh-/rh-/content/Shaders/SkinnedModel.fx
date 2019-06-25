#include "Include\\Common.fxh"

#define MaxBones 90

/************* Resources *************/
cbuffer CBufferPerFrame
{
    float4 AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
    float4 LightColor = { 0.7f, 0.7f, 0.7f, 0.9f };

	float3 LightPosition = { 0.f, 1.5f, -30.0f };
	float3 LightPosition2 = { -10.0f, 1.5f, -15.0f };
	float3 LightPosition3 = { -10.0f, 1.5f, 30.0f };
	float3 LightPosition4 = { -10.0f, 1.5f, 45.0f };
	float3 LightPosition5 = { -10.0f, 1.5f, 60.0f };
	float3 LightPosition6 = { -10.0f, 1.5f, 75.0f };
	float3 LightPosition7 = { -10.0f, 1.5f, 90.0f };
	float3 LightPosition8 = { -10.0f, 1.5f, 105.0f };
	float3 LightPosition9 = { 0.0f, 1.5f, 115.0f };
	float3 LightPosition10 = { 20.0f, 1.5f, -15.0f };

    float LightRadius = 150.0f;
    float3 CameraPosition;
}

cbuffer CBufferPerObject
{
    float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
    float4x4 World : WORLD;
    float4 SpecularColor : SPECULAR = { 0.0f, 0.0f, 0.0f, 0.0f };
    float SpecularPower : SPECULARPOWER  = 0.0f;
	float4x4 View : VIEW;
	float4x4 Projection : PROJECTION;

	float4 Hitted : HITTED;
}

cbuffer CBufferSkinning
{
    float4x4 BoneTransforms[MaxBones];
}

Texture2D ColorTexture;

RasterizerState MyCull {
	FrontCounterClockwise = TRUE;
};

RasterizerState MyCullNot {
	FrontCounterClockwise = FALSE;
};



float4 LineColor = float4(1, 0, 0, 1);
float LineThickness = 5.53f;


SamplerState ColorSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

/************* Data Structures *************/

struct VS_INPUT
{
    float4 ObjectPosition : POSITION;
    float2 TextureCoordinate : TEXCOORD;
    float3 Normal : NORMAL;
    uint4 BoneIndices : BONEINDICES;
    float4 BoneWeights : WEIGHTS;	
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 TextureCoordinate : TEXCOORD0;
    float3 WorldPosition : TEXCOORD1;
    float Attenuation : TEXCOORD2;
};


struct VS_SHADOW_INPUT
{
	float4 ObjectPosition : POSITION;
	uint4 BoneIndices : BONEINDICES;
	float4 BoneWeights : WEIGHTS;
};

struct VS_SHADOW_OUTPUT
{
	float4 Position : SV_Position;
};


float3 countFastLight(LIGHT_CONTRIBUTION_DATA lightContributionData, float3 WorldPosition, float3 normal, float3 pos, float rad)
{
	float3 result = (float3)0;
	float3 tempColor;
	lightContributionData.LightDirection = get_light_data(pos, WorldPosition, rad);
	lightContributionData.LightColor = LightColor;
	tempColor = get_light_contribution(lightContributionData);
	tempColor = get_toonShading(lightContributionData.LightDirection, normal, tempColor);
	result = tempColor;
	
	return result;
}

/************* Vertex Shader *************/

VS_OUTPUT vertex_shader(VS_INPUT IN)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4x4 skinTransform = (float4x4)0;
    skinTransform += BoneTransforms[IN.BoneIndices.x] * IN.BoneWeights.x;
    skinTransform += BoneTransforms[IN.BoneIndices.y] * IN.BoneWeights.y;
    skinTransform += BoneTransforms[IN.BoneIndices.z] * IN.BoneWeights.z;
    skinTransform += BoneTransforms[IN.BoneIndices.w] * IN.BoneWeights.w;
    

    float4 position = mul(IN.ObjectPosition, skinTransform);	
    OUT.Position = mul(position, WorldViewProjection);
    OUT.WorldPosition = mul(position, World).xyz;
    
    float4 normal = mul(float4(IN.Normal, 0), skinTransform);
    OUT.Normal = normalize(mul(normal, World).xyz);
    
    OUT.TextureCoordinate = IN.TextureCoordinate;

    float3 lightDirection = LightPosition - OUT.WorldPosition;
    OUT.Attenuation = saturate(1.0f - (length(lightDirection) / LightRadius));

    return OUT;
}

/************* Pixel Shaders *************/

float4 pixel_shader(VS_OUTPUT IN) : SV_Target
{
    float4 OUT = (float4)0;

	float3 normal = normalize(IN.Normal);
	float3 viewDirection = normalize(CameraPosition - IN.WorldPosition);
	float4 color = ColorTexture.Sample(ColorSampler, IN.TextureCoordinate);


	LIGHT_CONTRIBUTION_DATA lightContributionData;
	lightContributionData.Color = color;
	lightContributionData.Normal = normal;
	lightContributionData.ViewDirection = viewDirection;
	lightContributionData.SpecularColor = SpecularColor;
	lightContributionData.SpecularPower = SpecularPower;

	float3 totalLightContribution = (float3)0;
	
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition, 150);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition2, 150);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition3, 150);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition4, 100);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition5, 100);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition6, 100);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition7, 100);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition8, 100);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition9, 100);
	totalLightContribution += countFastLight(lightContributionData, IN.WorldPosition, normal, LightPosition10, 150);

	float3 diffuse = totalLightContribution;

	if (Hitted.x == 1.0f)
	{
		diffuse = float3(7.0f, 0.0f, 0.0f);
	}
	else if (Hitted.x == 0.5f)
	{
		diffuse = float3(0.0f, 1.0f, 0.0f);
	}


	OUT.rgb = diffuse;
    OUT.a = 1.0f;

    return OUT;
}

// temporary off
VS_OUTPUT OutlineVertexShader(VS_INPUT IN)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	float4x4 skinTransform = (float4x4)0;
    skinTransform += BoneTransforms[IN.BoneIndices.x] * IN.BoneWeights.x;
    skinTransform += BoneTransforms[IN.BoneIndices.y] * IN.BoneWeights.y;
    skinTransform += BoneTransforms[IN.BoneIndices.z] * IN.BoneWeights.z;
    skinTransform += BoneTransforms[IN.BoneIndices.w] * IN.BoneWeights.w;
    
	//float4 original = mul(mul(mul(IN.ObjectPosition, World), View), Projection);
	float4 normal = mul(mul(mul(IN.Normal, World), View), Projection);

    float4 position = mul(IN.ObjectPosition, skinTransform);	
    OUT.Position = mul(position, WorldViewProjection) + (mul(LineThickness, normal));

	return OUT;
}

// temporary off
float4 OutlinePixelShader(VS_OUTPUT IN) : SV_Target
{
	return LineColor;
}



VS_SHADOW_OUTPUT ShadowVertexShader(VS_SHADOW_INPUT IN)
{
	VS_SHADOW_OUTPUT OUT = (VS_SHADOW_OUTPUT)0;

	float4x4 skinTransform = (float4x4)0;
	skinTransform += BoneTransforms[IN.BoneIndices.x] * IN.BoneWeights.x;
	skinTransform += BoneTransforms[IN.BoneIndices.y] * IN.BoneWeights.y;
	skinTransform += BoneTransforms[IN.BoneIndices.z] * IN.BoneWeights.z;
	skinTransform += BoneTransforms[IN.BoneIndices.w] * IN.BoneWeights.w;

	float4 position = mul(IN.ObjectPosition, skinTransform);
	OUT.Position = mul(position, WorldViewProjection);

	return OUT;
}

float4 ShadowPixelShader(VS_SHADOW_OUTPUT IN) : SV_Target
{
	float4 OUT = float4(0.0, 0.0, 0.0, 1.0);
	return OUT;
}


/************* Techniques *************/

technique11 mainRender
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, vertex_shader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, pixel_shader()));
	}
}

technique11 mainShadow
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, ShadowVertexShader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ShadowPixelShader()));
	}
}