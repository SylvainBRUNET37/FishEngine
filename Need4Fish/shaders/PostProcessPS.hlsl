#include "VignettePS.hlsl"
#include "ChromaticAberrationPS.hlsl"
#include "Constants.hlsl"

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
    float closestEnnemiDistance;
    int enableChromaticAberration;
    float deltaTime;
    float pad0;
    float4x4 invProjection; // unused
    float4x4 invView; // unused
    float4x4 viewProj; // unused
    float3 cameraPos;
    float pad1;
	float3 sceneColorTint;
    float pad2;
};

Texture2D sceneTex : register(t0);
Texture2D specialMask : register(t1);
SamplerState sampleState : register(s0);

// =====================================
// Algorithm
// =====================================

static const float2 LIGHT_SCREEN_POS = float2(0.5, 0.05);
static const float GOD_RAY_LENGTH = 1.0;
static const float GOD_RAY_DECAY = 0.97;
static const float GOD_RAY_WEIGHT = 0.15;
static const float GOD_RAY_INTENSITY = 0.5;

static const int GOD_RAY_SAMPLES = 64;

float3 ComputeGodRays(float2 uv)
{
	// Direction from current pixel toward the light source
    float2 pixelToLightDir = LIGHT_SCREEN_POS - uv;
    pixelToLightDir *= GOD_RAY_LENGTH / GOD_RAY_SAMPLES;

    float2 sampleUV = uv;
    float illumination = 0.0;
    float decay = 1.0;

    // March toward the light to accumulate illumination
    [unroll]
    for (int i = 0; i < GOD_RAY_SAMPLES; i++)
    {
        sampleUV += pixelToLightDir;
        sampleUV = saturate(sampleUV);

        // Sample the mask (1 = pass, 0 = block)
		// Only the water plane has a r component so we ue this one
        float mask = specialMask.Sample(sampleState, sampleUV).r;

        illumination += mask * decay * GOD_RAY_WEIGHT;
        decay *= GOD_RAY_DECAY;
    }

    return illumination * (GOD_RAY_INTENSITY - sceneColorTint.r * 3.5);
}

float2 HeatwaveDistortion(float2 uv, float time)
{
    float speed = 6.0 + sin(time * 1.0) * 3.0;

    float wave1 = sin(uv.y * 70.0 + time * speed) * (0.012 + 0.01 * sin(time * 1.6));
    float wave2 = cos(uv.x * 55.0 + time * (speed * 0.9)) * (0.012 + 0.01 * cos(time * 2.0));

    float noise = sin((uv.x + uv.y) * 120.0 + time * 7.0) * 0.006;

    return float2(wave1 + noise, wave2 - noise);
}

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 color = sceneTex.Sample(sampleState, uv);

    // Apply distortion
    // It applies only one "distortion meshes" since they are the only onces with a g > 0.9
    float3 mask = specialMask.Sample(sampleState, uv).rgb;
    if (mask.g > 0.9)
    {
        float2 distortion = HeatwaveDistortion(uv, deltaTime);
        float timeIntensity = 0.5 + 0.3 * sin(deltaTime * 6.0);
        float2 distortedUV = uv + distortion * mask * timeIntensity;

    	color = sceneTex.Sample(sampleState, distortedUV);
    }

    if (enableChromaticAberration)
        color = ComputeChromaticAberration(sceneTex, sampleState, uv);

    // Add god rays from the water surface
    if (cameraPos.y < WATER_HEIGHT)
    {
        float3 godRays = ComputeGodRays(uv);
        color.rgb += godRays; 
    }
	else
	{
        color += 0.1; // add a little bit of intsensity if not in the water
    }

    color.rgb += sceneColorTint;

	color = ApplyVignette(color, uv, closestEnnemiDistance);

    return color;
}

