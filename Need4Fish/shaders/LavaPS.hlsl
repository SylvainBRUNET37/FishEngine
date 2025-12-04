// https://thebookofshaders.com/
// https://thebookofshaders.com/13
#include "MainBuffers.hlsl"

Texture2D sceneTex : register(t0);
SamplerState samp : register(s0);

// =====================================
// Includes
// =====================================

#include "BlingPhongBasePS.hlsl"
#include "UnderwaterAttenuationPS.hlsl"
#include "UnderwaterFogPS.hlsl"

// =====================================
// Algorithm
// =====================================

// Random float between 0 and 1
float Random(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453123);
}

// https://gamedev.stackexchange.com/questions/198247/creating-large-perlin-noise-spaces-on-gpu
float Noise(float2 p)
{
	float2 i = floor(p);
	float2 f = frac(p);

	float a = Random(i);
	float b = Random(i + float2(1.0, 0.0));
	float c = Random(i + float2(0.0, 1.0));
	float d = Random(i + float2(1.0, 1.0));

	float2 u = f * f * (3.0 - 2.0 * f);

	return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

float2 LavaDistortion(float2 uv)
{
	float noiseX = Noise(uv);
	float noiseY = Noise(uv);
	
	static const float STRENGTH = 0.1;
	return uv + float2(noiseX, noiseY) * STRENGTH;
}

// Paint the color of the pixel depending on the noise value
float3 LavaColor(float noiseValue)
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

static const float3 NOISE_FREQUENCE = float3(3.0, 6.0, 9.0);
static const float3 NOISE_SPEED = float3(0.5, -1.0, 1.5);
static const float3 NOISE_AMPLITUDE = float3(0.45, 0.30, 0.15);

float4 LavaPS(VSOutput input) : SV_Target
{
    // Distord UV
	float2 distordedUV = LavaDistortion(input.textCoord * 2.0);

    // Add noise with 3 octaves
	float noiseValue =
	    Noise(distordedUV * NOISE_FREQUENCE.x + elapsedTime * NOISE_SPEED.x) * NOISE_AMPLITUDE.x +
	    Noise(distordedUV * NOISE_FREQUENCE.y + elapsedTime * NOISE_SPEED.y) * NOISE_AMPLITUDE.y +
	    Noise(distordedUV * NOISE_FREQUENCE.z + elapsedTime * NOISE_SPEED.z) * NOISE_AMPLITUDE.z;

    // Convert the noise to a color
	float3 finalColor = LavaColor(noiseValue);

    // Add a glossy affect
	static const float GLOSSY_POWER = 2.5;
	finalColor *= GLOSSY_POWER;

    // Apply underwater effects if under water
	if (vCamera.y < WATER_HEIGHT)
	{
		finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz);
		finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz);
	}

	return float4(finalColor, 1.0);
}



