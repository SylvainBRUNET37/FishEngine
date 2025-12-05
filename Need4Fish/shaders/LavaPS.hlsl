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
float3 ApplyLavaColor(float noiseValue)
{
    // Lava colors
	static const float3 COLOR_BLACK = float3(0.0, 0.0, 0.0);
	static const float3 COLOR_RED = float3(0.9, 0.1, 0.0);
	static const float3 COLOR_ORANGE = float3(1.0, 0.4, 0.0);
	static const float3 COLOR_YELLOW = float3(1.0, 0.9, 0.3);
	static const float3 COLOR_WHITE = float3(1.0, 1.0, 1.0);

    // End value range of color gradiant
	static const float GRADIANT_SEGMENT_WIDTH = 0.25;

	static const float RED_STOP = 0.25;
	static const float ORANGE_STOP = RED_STOP + GRADIANT_SEGMENT_WIDTH;
	static const float YELLOW_STOP = ORANGE_STOP + GRADIANT_SEGMENT_WIDTH;

	if (noiseValue < RED_STOP) // [0, 0.25] = Black -> Red
		return lerp(COLOR_BLACK, COLOR_RED, noiseValue / GRADIANT_SEGMENT_WIDTH);

	if (noiseValue < ORANGE_STOP) // [0, 0.50] = Red -> Orange
		return lerp(COLOR_RED, COLOR_ORANGE, (noiseValue - RED_STOP) / GRADIANT_SEGMENT_WIDTH);

	if (noiseValue < YELLOW_STOP) // [0, 0.75] = Orange -> Yellow
		return lerp(COLOR_ORANGE, COLOR_YELLOW, (noiseValue - ORANGE_STOP) / GRADIANT_SEGMENT_WIDTH);

	// Yellow -> White
	return lerp(COLOR_YELLOW, COLOR_WHITE, (noiseValue - YELLOW_STOP) / GRADIANT_SEGMENT_WIDTH);
}

float4 LavaPS(VSOutput input) : SV_Target
{
    // Get a noise value
	float noiseValue = CreateNoise(input.textCoord, elapsedTime);

    // Convert the noise to a color
	float3 finalColor = ApplyLavaColor(noiseValue);

    // Add a glossy affect
	static const float GLOSSY_POWER = 2.5;
	finalColor *= GLOSSY_POWER;

    // Apply underwater effects if under water
	if (vCamera.y < WATER_HEIGHT)
	{
        finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz, WATER_HEIGHT);
        finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz, WATER_HEIGHT);
    }

	return float4(finalColor, 1.0);
}
