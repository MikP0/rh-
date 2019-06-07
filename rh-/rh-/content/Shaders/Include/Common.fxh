#ifndef _COMMON_FXH
#define _COMMON_FXH

/************* Constants *************/

#define FLIP_TEXTURE_Y 0

/************* Data Structures *************/

struct DIRECTIONAL_LIGHT
{
	float4 Color;
	float3 Direction;   
	float EmptyTo16B;
};

struct POINT_LIGHT
{
	float3 Position : POSITION;
	float Radius;
	float4 Color : COLOR;
};

struct SPOT_LIGHT
{
	float4 Color;
	float3 Direction;
	float OuterAngle;
	float3 Position;	
	float InnerAngle;
	float Radius;
	float3 SpaceTo16B;
};

struct LIGHT_CONTRIBUTION_DATA
{
    float4 Color;
	float3 Normal;
	float3 ViewDirection;
	float4 LightColor;
	float4 LightDirection;	
	float4 SpecularColor;
	float SpecularPower;	
};

/************* Utility Functions *************/

float2 get_corrected_texture_coordinate(float2 textureCoordinate)
{
	#if FLIP_TEXTURE_Y
		return float2(textureCoordinate.x, 1.0 - textureCoordinate.y); 
	#else
    	return textureCoordinate; 
	#endif
}

float3 get_vector_color_contribution(float4 light, float3 color)
{
	// Color (.rgb) * Intensity (.a)
	return light.rgb * light.a * color;
}

float3 get_scalar_color_contribution(float4 light, float color)
{
	// Color (.rgb) * Intensity (.a)
	return light.rgb * light.a * color;
}

float4 get_light_data(float3 lightPosition, float3 worldPosition, float lightRadius)
{
	float4 lightData;
	float3 lightDirection = lightPosition - worldPosition;

	lightData.xyz = normalize(lightDirection);
	lightData.w = saturate(1.0f - length(lightDirection) / lightRadius); // Attenuation

	return lightData;
}

float3 get_light_contribution(LIGHT_CONTRIBUTION_DATA IN)
{
	float3 lightDirection = IN.LightDirection.xyz;	
	float n_dot_l = dot(IN.Normal, lightDirection);	
	float3 halfVector = normalize(lightDirection + IN.ViewDirection);
	float n_dot_h = dot(IN.Normal, halfVector);
	
	float4 lightCoefficients = lit(n_dot_l, n_dot_h, IN.SpecularPower);
	float3 diffuse = get_vector_color_contribution(IN.LightColor, lightCoefficients.y * IN.Color.rgb) * IN.LightDirection.w;
	float3 specular = get_scalar_color_contribution(IN.SpecularColor, min(lightCoefficients.z, IN.Color.w)) * IN.LightDirection.w;
	
	return (diffuse + specular);
}

float3 get_spot_light(SPOT_LIGHT SP, float4 color, float3 normal, float3 WorldPos, float3 view, float4 SpecularColor, float SpecularPower)
{
	float3 LightLookAt = normalize(-SP.Direction);
	float3 lightDirection = (SP.Position - WorldPos);
	float attenuation = saturate(1.0f - (length(lightDirection) / SP.Radius));

	lightDirection = normalize(SP.Position - WorldPos);

	float n_dot_l = dot(normal, lightDirection);
	float3 halfVector = normalize(lightDirection + view);
	float n_dot_h = dot(normal, halfVector);
	float3 lightLookAt = normalize(LightLookAt);

	float spotFactor = 0.0f;
	float lightAngle = dot(lightLookAt, lightDirection);
	if (lightAngle > 0.0f)
	{
		spotFactor = smoothstep(SP.OuterAngle, SP.InnerAngle, lightAngle);
	}

	float4 lightCoefficients = lit(n_dot_l, n_dot_h, SpecularPower);

	float3 diffuse = get_vector_color_contribution(SP.Color, lightCoefficients.y * color.rgb) * attenuation;
	float3 specular = get_scalar_color_contribution(SpecularColor, min(lightCoefficients.z, color.w)) * attenuation;

	float3 result = (spotFactor * (diffuse + specular));

	return result;
}


float get_fog_amount(float3 eyePosition, float3 worldPosition, float fogStart, float fogRange)
{
	return saturate((length(eyePosition - worldPosition) - fogStart) / (fogRange));
}

float get_fog_amount(float3 viewDirection, float fogStart, float fogRange)
{
	return saturate((length(viewDirection) - fogStart) / (fogRange));
}

float3 get_toonShading(float4 LightDirection, float3 normal, float3 color)
{
	float3 toonColor = color;

	float intensity = dot(normalize(LightDirection), normal);
	if (intensity < 0)
		intensity = 0;

	if (intensity > 0.95)
		toonColor = float3(1.0, 1.0, 1.0) * toonColor;
	else if (intensity > 0.8)
		toonColor = float3(0.8, 0.8, 0.8) * toonColor;
	else if (intensity > 0.6)
		toonColor = float3(0.6, 0.6, 0.6) * toonColor;
	else if (intensity > 0.4)
		toonColor = float3(0.4, 0.4, 0.4) * toonColor;
	else if (intensity > 0.2)
		toonColor = float3(0.2, 0.2, 0.2) * toonColor;
	else
		toonColor = float3(0.1, 0.1, 0.1) * toonColor;

	return toonColor;
}


#endif /* _COMMON_FXH */