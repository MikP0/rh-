//---------------------------------------------------------------------------------------
// Performs shadowmap test to determine if a pixel is in shadow.
//---------------------------------------------------------------------------------------

static const float SMAP_SIZE = 1024.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;
#define SHADOW_EPSILON 0.00005f

float CalcShadowFactor(
	sampler samShadow,
	Texture2D<float2> shadowMap,
	float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	//transform from RT space to texture space.
	float2 ShadowTexC = 0.5 * shadowPosH.xy + float2(0.5, 0.5);
	ShadowTexC.y = 1.0f - ShadowTexC.y;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	// Texel size.
	const float dx = SMAP_DX;

	float2 mapDepth = shadowMap.Sample(samShadow, ShadowTexC).xy;

	float	fPercentLit = 0.0f;
	float  fAvgZ = mapDepth.x; // Filtered z
	float  fAvgZ2 = mapDepth.y; // Filtered z-squared

	if (depth <= fAvgZ) // We put the z value in w so that we can index the texture array with Z.
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

		// To combat light-bleeding, experiment with raising p_max to some power
		// (Try values from 0.1 to 100.0, if you like.)
		fPercentLit = pow(p_max, 50);
	}

	return fPercentLit;
}