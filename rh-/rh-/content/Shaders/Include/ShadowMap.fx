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

	float fPercentLit = 0.0f;
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
}

/*
static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
	Texture2D shadowMap,
	float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	// Texel size.
	const float dx = SMAP_DX;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMap.SampleCmpLevelZero(samShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit /= 9.0f;
}*/