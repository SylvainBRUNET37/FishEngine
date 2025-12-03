cbuffer ObjectBuffer : register(b1)
{
	float4x4 world;
};

cbuffer FrameBuffer : register(b0)
{
	float4x4 viewProj;
};

struct VSInput
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VSOutput DistVS(VSInput input)
{
	VSOutput output;

	float4 worldPos = mul(float4(input.pos, 1.0f), world);
	output.pos = mul(worldPos, viewProj);

	output.uv = input.uv;
	return output;
}
