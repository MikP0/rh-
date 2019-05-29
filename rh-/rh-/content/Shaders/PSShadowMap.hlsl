cbuffer DynamicBuffer : register(b1)
{
	float4x4 WorldViewProjection;
};


Texture2D ColorMap : register(t0);

//------------------------------------------------------------------------------
// Per-pixel color data passed through the pixel shader.
//------------------------------------------------------------------------------
struct PixelShaderInput
{
	float4 pos : SV_Position;
	float2 texCoord : TEXCOORD0;
};

SamplerState ColorSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};


float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = ColorMap.Sample(ColorSampler, input.texCoord);

	clip(color.a - 0.15f);

	return color;
}