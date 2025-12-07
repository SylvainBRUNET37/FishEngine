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
    float4x4 invProjection;
    float4x4 invView;
    float4x4 viewProj;
    int pad1;
};

Texture2D sceneTex : register(t0);
Texture2D distortionMask : register(t1);
Texture2D<float> depthTex : register(t2);
SamplerState sampleState : register(s0);

// =====================================
// Algorithm
// =====================================

static const float nearClip = 1.0f;
static const float farClip = 16000.0f;

float LinearizeDepth(float depth)
{
    return ((nearClip * farClip) / (farClip - depth * (farClip - nearClip))) / farClip;
}

float3 GetWorldPos(float2 uv, float depth)
{
    // Convert UV + depth to clip space
    float4 clipPos = float4
	(
        uv * 2.0f - 1.0f, // UV to NDC
        depth,
        1.0f
    );

    // Clip to view
    float4 viewPos = mul(invProjection, clipPos);
    viewPos /= viewPos.w;

    // View to world
    float4 worldPos = mul(invView, viewPos);
    return worldPos.xyz;
}

float2 HeatwaveDistortion(float2 uv, float time)
{
    float speed = 3.0 + sin(time * 0.7) * 1.5;

    float wave1 = sin(uv.y * 45.0 + time * speed) * (0.005 + 0.005 * sin(time * 1.3));
    float wave2 = cos(uv.x * 30.0 + time * (speed * 0.8)) * (0.005 + 0.005 * cos(time * 1.7));

    float noise = sin((uv.x + uv.y) * 80.0 + time * 5.0) * 0.003;

    return float2(wave1 + noise, wave2 - noise);
}

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 color = sceneTex.Sample(sampleState, uv);

    float mask = distortionMask.Sample(sampleState, uv).r;
    if (mask > 0.0)
    {
        float2 distortion = HeatwaveDistortion(uv, deltaTime);
        float timeIntensity = 1.0 + 0.3 * sin(deltaTime * 6.0);
        float2 distortedUV = uv + distortion * mask * timeIntensity;

        if (distortionMask.Sample(sampleState, distortedUV).r > 0.0)
            color = sceneTex.Sample(sampleState, distortedUV);
    }

    if (enableChromaticAberration)
        color = ComputeChromaticAberration(sceneTex, sampleState, uv);

    if (enableVignette)
        color = ApplyVignette(color, uv);

    return color;
}

