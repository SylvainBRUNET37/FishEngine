#include "MainBuffers.hlsl"

Texture2D sceneTex : register(t0);
SamplerState samp : register(s0);

// =====================================
// Includes
// =====================================

#include "UnderwaterAttenuationPS.hlsl"
#include "UnderwaterFogPS.hlsl"
#include "Noise.hlsl"

// =====================================
// Algorithm
// =====================================

// Paint the pixel with a color which depend on the noise value
float3 ApplyWaterColor(float noiseValue)
{
	// Water colors
    static const float3 COLOR_DEEP = float3(0.15, 0.35, 0.70);
    static const float3 COLOR_DARKBLUE = float3(0.25, 0.55, 0.85);
    static const float3 COLOR_BLUE = float3(0.55, 0.80, 1.00);

	// Gradient segment width
    static const float GRADIENT_SEGMENT_WIDTH = 0.5;

    if (noiseValue < GRADIENT_SEGMENT_WIDTH) // Deep -> DarkBlue
        return lerp(COLOR_DEEP, COLOR_DARKBLUE, noiseValue / GRADIENT_SEGMENT_WIDTH);

    // DarkBlue -> Blue
    return lerp(COLOR_DARKBLUE, COLOR_BLUE, (noiseValue - GRADIENT_SEGMENT_WIDTH) / GRADIENT_SEGMENT_WIDTH);
}

float4 WaterPS(VSOutput input) : SV_Target
{
    // Get a noise value
	float noiseValue = CreateNoise(input.textCoord, elapsedTime);

    // Convert the noise to a color
	float3 finalColor = ApplyWaterColor(noiseValue);

	// Add anti glossy affect
    static const float GLOSSY_POWER = 0.8;
    finalColor *= GLOSSY_POWER;

    // Apply underwater effects if under water
	if (vCamera.y < WATER_HEIGHT)
	{
        finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz, WATER_HEIGHT);
        finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz, WATER_HEIGHT);
    }

	return float4(finalColor, 1.0);
}
