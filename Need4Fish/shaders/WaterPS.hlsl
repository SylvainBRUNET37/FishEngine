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
	static const float3 COLOR_DEEP = float3(0.0, 0.0, 0.2);
	static const float3 COLOR_BLUE = float3(0.0, 0.2, 0.8);
	static const float3 COLOR_CYAN = float3(0.0, 0.8, 1.0);
	static const float3 COLOR_WHITE = float3(0.8, 0.9, 1.0);

	// Gradient segment width
	static const float GRADIENT_SEGMENT_WIDTH = 0.25;

	static const float DEEP_STOP = 0.25;
	static const float BLUE_STOP = DEEP_STOP + GRADIENT_SEGMENT_WIDTH;
	static const float CYAN_STOP = BLUE_STOP + GRADIENT_SEGMENT_WIDTH;

	if (noiseValue < DEEP_STOP) // [0, 0.25] = Deep -> Blue
		return lerp(COLOR_DEEP, COLOR_BLUE, noiseValue / GRADIENT_SEGMENT_WIDTH);

	if (noiseValue < BLUE_STOP) // [0.25, 0.50] = Blue -> Cyan
		return lerp(COLOR_BLUE, COLOR_CYAN, (noiseValue - DEEP_STOP) / GRADIENT_SEGMENT_WIDTH);

	if (noiseValue < CYAN_STOP) // [0.50, 0.75] = Cyan -> White
		return lerp(COLOR_CYAN, COLOR_WHITE, (noiseValue - BLUE_STOP) / GRADIENT_SEGMENT_WIDTH);

	// White
	return COLOR_WHITE;
}

float4 WaterPS(VSOutput input) : SV_Target
{
    // Get a noise value
	float noiseValue = CreateNoise(input.textCoord, elapsedTime);

    // Convert the noise to a color
	float3 finalColor = ApplyWaterColor(noiseValue);

    // Apply underwater effects if under water
	if (vCamera.y < WATER_HEIGHT)
	{
		finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz);
		finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz);
	}

	return float4(finalColor, 1.0);
}
