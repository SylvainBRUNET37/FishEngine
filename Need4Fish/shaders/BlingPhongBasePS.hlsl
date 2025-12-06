// =====================================
// Includes
// =====================================

#include "LightningPS.hlsl"

// =====================================
// Algorithm
// =====================================

float3 ApplyBlingPhong(VSOutput input) : SV_Target
{
	float3 surfaceNormal = normalize(input.normal);
	float3 viewDirection = normalize(vCamera.xyz - input.worldPosition);

	float3 finalColor = float3(0, 0, 0);

    // Directional light
	finalColor += ComputeDirLight(surfaceNormal, viewDirection);

    // Point lights
	for (int i = 0; i < pointLightCount; i++)
	{
		finalColor += ComputePointLight(pointLights[i], surfaceNormal, viewDirection, input.worldPosition);
	}

    // Apply texture
	if (bTex > 0.5f)
	{
		float3 texColor = sceneTex.Sample(samp, input.textCoord).rgb;
		finalColor *= texColor;
	}

	return finalColor;
}

float3 ApplyBlingPhong(VSOutput input, SamplerComparisonState samShadow, Texture2D gShadowMap, float4 shadowPosH) : SV_Target
{
    float3 surfaceNormal = normalize(input.normal);
    float3 viewDirection = normalize(vCamera.xyz - input.worldPosition);

    float3 finalColor = float3(0, 0, 0);

    // Directional light
    finalColor += ComputeDirLight(surfaceNormal, viewDirection, samShadow, gShadowMap, shadowPosH);

    // Point lights
    for (int i = 0; i < pointLightCount; i++)
    {
        finalColor += ComputePointLight(pointLights[i], surfaceNormal, viewDirection, input.worldPosition);
    }

    // Apply texture
    if (bTex > 0.5f)
    {
        float3 texColor = sceneTex.Sample(samp, input.textCoord).rgb;
        finalColor *= texColor;
    }

    return finalColor;
}
