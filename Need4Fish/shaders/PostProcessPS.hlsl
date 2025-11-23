// =====================================
// Inputs / Outputs
// =====================================

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D sceneTexture : register(t0);
SamplerState sampleState : register(s0);

// =====================================
// Vignette
// =====================================

// Harcoded vignette parameters, could be given with CB if needed
// Vignette effect: https://www.youtube.com/watch?v=GiQ5OvDN8dE
static const float VIGNETTE_RADIUS = 0.8; // distance from the center where the effect begins
static const float VIGNETTE_SOFTNESS = 0.5; // transition softness between the zone with and without the effect
static const float VIGNETTE_STRENGTH = 0.3; // intensity of the color (it's like an alpha channel)
static const float4 VINGETTE_COLOR = float4(0.0, 0.0, 0.0, 1.0);

float4 ApplyVignette(float4 textureColor, float2 uv)
{
    const float2 centered = uv * 2.0 - 1.0;
    const float distanceFromCenter = length(centered);

    // Compute how far into the vignette the pixel is, and use that value to darken the image toward the edges
    const float vignetteMask = smoothstep(VIGNETTE_RADIUS, VIGNETTE_RADIUS + VIGNETTE_SOFTNESS, distanceFromCenter);
    const float darkFactor = lerp(1.0, 1.0 - VIGNETTE_STRENGTH, vignetteMask);

    // Tint the vignette with the color
    const float4 tintedColor = lerp(textureColor, VINGETTE_COLOR, vignetteMask * VIGNETTE_STRENGTH);

    // Combine tint + darkening
    const float4 finalColor = float4(tintedColor.rgb * darkFactor, textureColor.a);

    return finalColor;
}

// =====================================
// Chromatic Aberration
// =====================================

// Hardcoded chromatic aberration values
// Chromatic aberration effect: https://lettier.github.io/3d-game-shaders-for-beginners/chromatic-aberration.html
static const float2 CA_RED_OFFSET = float2(0.01, 0.0);
static const float2 CA_GREEN_OFFSET = float2(0.02, 0.0);
static const float2 CA_BLUE_OFFSET = float2(-0.01, 0.0);

float4 ComputeChromaticAberration(float2 uv)
{
    const float r = sceneTexture.Sample(sampleState, uv + CA_RED_OFFSET).r;
    const float g = sceneTexture.Sample(sampleState, uv + CA_GREEN_OFFSET).g;
    const float b = sceneTexture.Sample(sampleState, uv + CA_BLUE_OFFSET).b;

    return float4(r, g, b, 1.0);
}

float4 PostProcessPS(VSOutput input) : SV_TARGET
{
	// This code apply Vingette + CA
    // const float4 chromaColor = ComputeChromaticAberration(input.uv);
    // return ApplyVignette(chromaColor, input.uv);

    // Apply only Vignette effect
    float4 pixelColor = sceneTexture.Sample(sampleState, input.uv);
    return ApplyVignette(pixelColor, input.uv);
}
