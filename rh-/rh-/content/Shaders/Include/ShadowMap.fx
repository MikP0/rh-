//---------------------------------------------------------------------------------------
// Performs shadowmap test to determine if a pixel is in shadow.
//---------------------------------------------------------------------------------------

static const float SMAP_SIZE = 1024.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(
	SamplerState samShadow,
	Texture2D<float2> shadowMap,
	float4 shadowPosH)
{
	shadowPosH.xyz /= shadowPosH.w;

	float2 ShadowTexC = 0.5 * shadowPosH.xy + float2(0.5, 0.5);
	ShadowTexC.y = 1.0f - ShadowTexC.y;

	float depth = shadowPosH.z;

	const float dx = SMAP_DX;

	float2 mapDepth = shadowMap.Sample(samShadow, ShadowTexC).xy;

	//

	float fAvgZ = mapDepth.x;
	float fAvgZ2 = mapDepth.y;

	float variance = 1;

	if (depth > fAvgZ)
	{
		variance = (fAvgZ2)-(fAvgZ * fAvgZ);
		variance = min(1.0f, max(0.0f, variance + 0.00001f));
	}

	float shadow = 0.0;
	float2 texelSize = 1.0 / 1024.0f;;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = shadowMap.Sample(samShadow, ShadowTexC.xy + float2(x, y) * texelSize).xy;
			shadow += depth - variance > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (shadowPosH.z > 1.0)
		shadow = 0.0;

	shadow = 1 - shadow;

	return shadow;
}

/*float fPercentLit = 0.0f;
	float fAvgZ = mapDepth.x;
	float fAvgZ2 = mapDepth.y;

	if (depth <= fAvgZ)
	{
		fPercentLit = 1;
	}
	else
	{
		float variance = (fAvgZ2)-(fAvgZ * fAvgZ);
		variance = min(1.0f, max(0.0f, variance + 0.00001f));

		float mean = fAvgZ;
		float d = depth - mean;
		float p_max = variance / (variance + d * d);

		fPercentLit = pow(p_max, 4);
	}

	return fPercentLit;
*/