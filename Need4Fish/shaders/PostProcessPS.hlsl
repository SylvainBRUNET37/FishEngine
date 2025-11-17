// =====================================
// Constant buffers datas
// =====================================

cbuffer VignetteParams : register(b0)
{
	float radius; // Where effect begins
	float softness; // Smoothness of the falloff
	float strength; // Darkness strength
	float2 padding; // Alignement
};

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

Texture2D sceneTexture : register(t0);
SamplerState samp : register(s0);

// =====================================
// Algorithm
// =====================================

float4 PostProcessPS(VSInput input) : SV_TARGET
{
    float2 pos = input.uv * 2.0 - 1.0;
	float dist = length(pos);

    // Vignette falloff
	float vignette = smoothstep(radius, radius - softness, dist);

    float4 color = sceneTexture.Sample(samp, input.uv);

    // Darken edges
	color.rgb *= lerp(1.0, 1.0 - strength, vignette);

	return color;
}
