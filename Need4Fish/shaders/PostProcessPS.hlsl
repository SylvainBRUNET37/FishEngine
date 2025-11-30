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
    float deltaTime;
    int pad1;
};

Texture2D sceneTex : register(t0);
Texture2D distortionMask : register(t1);
SamplerState sampleState : register(s0);

// =====================================
// Algorithm
// =====================================

float2 HeatwaveDistortion(float2 uv, float time)
{
    // Animated distortion effect
    float wave1 = sin(uv.y * 40.0 + time * 4.0) * 0.02;
    float wave2 = cos(uv.x * 25.0 + time * 3.0) * 0.02;

    return float2(wave1, wave2);
}

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv;

    float4 color = sceneTex.Sample(sampleState, uv);

    float mask = distortionMask.Sample(sampleState, uv).r;
    if (mask > 0.0)
    {
        // Apply distortion inside the mask
        float2 distortion = HeatwaveDistortion(uv, deltaTime);
        float2 distortedUV = uv + distortion * mask;

        float maskDistorted = distortionMask.Sample(sampleState, distortedUV).r;
        if (maskDistorted > 0.0)
            color = sceneTex.Sample(sampleState, distortedUV);
    }

    if (enableChromaticAberration)
        color = ComputeChromaticAberration(sceneTex, sampleState, uv);

    if (enableVignette)
        color = ApplyVignette(color, uv);

    return color;
}

