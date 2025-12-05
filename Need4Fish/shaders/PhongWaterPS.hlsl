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

float4 PhongWaterPS(VSOutput input) : SV_Target
{
    float3 finalColor = ApplyBlingPhong(input);

	// Apply underwater effects if the camera is in the water
    if (vCamera.y < WATER_HEIGHT)
    {
        finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz, WATER_HEIGHT);
        finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz, WATER_HEIGHT);
    }

    return float4(finalColor, 1.0f);
}
