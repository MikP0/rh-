cbuffer DynamicBuffer : register(b1)
{
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
	float3 pos : SV_Position;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
};

struct PixelShaderInput
{
	float4 pos : SV_Position;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput OUT = (PixelShaderInput)0;

	float4 pos = mul(float4(input.pos, 1.0f), WorldViewProjection);

	OUT.pos = pos;

	return OUT;
}
