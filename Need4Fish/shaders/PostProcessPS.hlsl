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
Texture2D distortionTex : register(t1);
SamplerState sampleState : register(s0);

static const float DISTORTION_STRENGTH = 0.02;

// =====================================
// Algorithm
// =====================================

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 color = sceneTex.Sample(sampleState, uv);

    float glossMask = distortionTex.Sample(sampleState, uv).r;
    float glossIntensity = 0.1;
    float3 glossColor = float3(1.0, 1.0, 1.0);

    color.rgb += glossColor * glossMask * glossIntensity;

    if (enableChromaticAberration)
        color = ComputeChromaticAberration(sceneTex, sampleState, input.uv);

    if (enableVignette)
        color = ApplyVignette(color, input.uv);

    return color;
}
