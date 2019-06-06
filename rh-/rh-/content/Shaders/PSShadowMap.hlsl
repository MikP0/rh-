cbuffer DynamicBuffer : register(b1)
{
	float4x4 WorldViewProjection;
};

struct PixelShaderInput
{
	float4 pos : SV_Position;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return 0;
}