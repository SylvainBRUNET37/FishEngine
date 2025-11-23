#include "VignettePS.hlsl"
#include "ChromaticAberrationPS.hlsl"

// =====================================
// Inputs / Outputs
// =====================================

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// =====================================
// Constant buffers
// =====================================

cbuffer PostProcessSettings : register(b0)
{
    int enableVignette;
    int enableChromaticAberration;
    int pad1;
    int pad2;
};

Texture2D sceneTex : register(t0);
SamplerState sampleState : register(s0);

// =====================================
// Algorithm
// =====================================

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
    float4 color = sceneTex.Sample(sampleState, input.uv);

    if (enableChromaticAberration)
        color = ComputeChromaticAberration(sceneTex, sampleState, input.uv);

    if (enableVignette)
        color = ApplyVignette(color, input.uv);

    return color;
}
