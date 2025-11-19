// =====================================
// Inputs / Outputs
// =====================================

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// =====================================
// Algorithm
// =====================================

Texture2D sceneTexture : register(t0);
SamplerState samp : register(s0);

// Harcoded vignette parameters, could be given with CB if needed
// Vignette effect: https://www.youtube.com/watch?v=GiQ5OvDN8dE
static const float radius = 0.8; // distance from the center where the effect begins
static const float softness = 0.5; // transition softness between the zone with and without the effect
static const float strength = 0.5; // intensity of the color (it's like an alpha channel)

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
    float2 position = input.uv * 2.0 - 1.0;
    float distance = length(position);

    // The vignette effect
    float vignetteMask = smoothstep(radius, radius + softness, distance);

    // Retrieve the color of the pixel of the scene
    float4 vignetteColor = sceneTexture.Sample(samp, input.uv);

    // Apply vignette
    vignetteColor.rgb *= lerp(1.0, 1.0 - strength, vignetteMask);

    return vignetteColor;
}
