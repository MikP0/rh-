#include "Include\\Common.fxh"

#define MaxBones 90

/************* Resources *************/
cbuffer CBufferPerFrame
{
    float4 AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
    float4 LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float3 LightPosition = { 0.0f, 0.0f, -2.0f };
    float LightRadius = 100.0f;
    float3 CameraPosition;
}

cbuffer CBufferPerObject
{
    float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
    float4x4 World : WORLD;
    float4 SpecularColor : SPECULAR = { 1.0f, 1.0f, 1.0f, 1.0f };
    float SpecularPower : SPECULARPOWER  = 25.0f;
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

    float3 lightDirection = LightPosition - IN.WorldPosition;
    lightDirection = normalize(lightDirection);

	float4 color = ColorTexture.Sample(ColorSampler, IN.TextureCoordinate);

	// TOON SHADING
	float intensity = dot(normalize(lightDirection), IN.Normal);
	if (intensity < 0)
		intensity = 0;

	color.a = 1;

	/*if (intensity > 0.95)
		color = float4(1.0, 1, 1, 1.0) * color;
	else if (intensity > 0.5)
		color = float4(0.7, 0.7, 0.7, 1.0) * color;
	else if (intensity > 0.05)
		color = float4(0.35, 0.35, 0.35, 1.0) * color;
	else
		color = float4(0.1, 0.1, 0.1, 1.0) * color;*/



	/*if (intensity > 0.95)
		color = float4(1.0, 1.0, 1.0, 1.0) * color;
	else if (intensity > 0.8)
		color = float4(0.8, 0.8, 0.8, 1.0) * color;
	else if (intensity > 0.6)
		color = float4(0.6, 0.6, 0.6, 1.0) * color;
	else if (intensity > 0.4)
		color = float4(0.4, 0.4, 0.4, 1.0) * color;
	else if (intensity > 0.2)
		color = float4(0.2, 0.2, 0.2, 1.0) * color;
	else
		color = float4(0.1, 0.1, 0.1, 1.0) * color;*/



	float3 mycam;
	mycam.x = 0.0f;
	mycam.y = 0.0f;
	mycam.z = 0.0f;

    float3 viewDirection = normalize(mycam - IN.WorldPosition);

    float3 normal = normalize(IN.Normal);
    float n_dot_l = dot(normal, lightDirection);
    float3 halfVector = normalize(lightDirection + viewDirection);
    float n_dot_h = dot(normal, halfVector);

    float4 lightCoefficients = lit(n_dot_l, n_dot_h, SpecularPower);

    float3 ambient = get_vector_color_contribution(AmbientColor, color.rgb);
    float3 diffuse = get_vector_color_contribution(LightColor, lightCoefficients.y * color.rgb) * IN.Attenuation;
    float3 specular = get_scalar_color_contribution(SpecularColor, min(lightCoefficients.z, color.w)) * IN.Attenuation;

	if (Hitted.x == 1.0f)
	{
		diffuse = float3(7.0f, 0.0f, 0.0f);
	}
	else if (Hitted.x == 0.5f)
	{
		diffuse = float3(0.0f, 1.0f, 0.0f);
	}


	OUT.rgb = ambient + diffuse;// +specular;
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