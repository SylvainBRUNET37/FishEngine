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

Texture2D sceneTex : register(t0);
SamplerState sampleState : register(s0);

// =====================================
// Algorithm
// =====================================

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
	// This code apply Vingette + CA
    //const float4 chromaColor = ComputeChromaticAberration(sceneTex, sampleState, input.uv);
    //return ApplyVignette(chromaColor, input.uv);

    // Apply only Vignette effect
    float4 pixelColor = sceneTex.Sample(sampleState, input.uv);
    return ApplyVignette(pixelColor, input.uv);
}
