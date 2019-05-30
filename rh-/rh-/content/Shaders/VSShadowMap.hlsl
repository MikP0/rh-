cbuffer DynamicBuffer : register(b1)
{
	float4x4 WorldViewProjection;
};

//------------------------------------------------------------------------------
// Per-vertex data used as input to the vertex shader.
//------------------------------------------------------------------------------
struct VertexShaderInput
{
	float3 pos : SV_Position;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 color : COLOR;
	float2 texCoord : TEXCOORD0;
};

//------------------------------------------------------------------------------
// Per-pixel color data passed through the pixel shader.
//------------------------------------------------------------------------------
struct PixelShaderInput
{
	float4 pos : SV_Position;
	float2 texCoord : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput OUT = (PixelShaderInput)0;

	float4 pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
	float2 tex = input.texCoord;

	OUT.pos = pos;
	OUT.texCoord = tex;

	return OUT;
}
