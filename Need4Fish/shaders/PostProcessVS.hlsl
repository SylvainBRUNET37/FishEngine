// =====================================
// Inputs / Outputs
// =====================================

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

// =====================================
// Algorithm
// =====================================

VSOutput PostProcessVS(VSInput input)
{
	VSOutput output;

	output.pos = float4(input.pos, 1.0f);
	output.uv = input.uv;

	return output;
}
