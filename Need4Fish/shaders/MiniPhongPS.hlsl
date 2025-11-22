#include "UnderwaterAttenuation.hlsl"
#include "UnderwaterFog.hlsl"
#include "Caustics.hlsl"

// =====================================
// Constant buffers datas
// =====================================

struct DirectionLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 direction;
    float pad; // alignment
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 position;
    float range; // not used yet but may be useful in the future
    float3 attenuation;
    float pad; // alignment
};

#define MAX_POINT_LIGHTS 16

// Per-frame
cbuffer FrameBuffer : register(b0)
{
    float4x4 matViewProj;
    float4 vCamera; // camera pos
    DirectionLight dirLight;

    int pointLightCount;
    float elapsedTime;
    float2 padding_;

    PointLight pointLights[MAX_POINT_LIGHTS];
};

// Per-object
cbuffer ObjectBuffer : register(b1)
{
    float4x4 matWorld;
};

// Per-material
cbuffer MaterialBuffer : register(b2)
{
    float4 vAMat;
    float4 vDMat;
    float4 vSMat;
    float puissance;
    float bTex;
    float2 padding;
};

// =====================================
// Inputs / Outputs
// =====================================

struct VSOutput
{
    float4 position : SV_Position;
    float3 normal : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
    float2 textCoord : TEXCOORD2;
};

// =====================================
// Algorithm
// =====================================

static const float WATER_HEIGHT = 2620.0; // (TODO: do not hardcodeif possible)

Texture2D textureEntree : register(t0);
SamplerState SampleState : register(s0);

Texture2D causticTex : register(t1);
SamplerState causticSamp : register(s1);

float3 ComputeDirLight(float3 surfaceNormal, float3 viewDirection)
{
	// Calculate light direction
    float3 lightDirection = normalize(-dirLight.direction);
    float diffuseIntensity = saturate(dot(surfaceNormal, lightDirection));

    // Calculate specular reflection
    float3 reflectionDirection = reflect(-lightDirection, surfaceNormal);
    float specularIntensity = pow(saturate(dot(reflectionDirection, viewDirection)), puissance);

    // Combine material and light properties
    float3 ambient = dirLight.ambient.rgb * vAMat.rgb;
    float3 diffuse = dirLight.diffuse.rgb * vDMat.rgb * diffuseIntensity;
    float3 specular = dirLight.specular.rgb * vSMat.rgb * specularIntensity;

    return ambient + diffuse + specular;
}

float3 ComputePointLight(PointLight pointLight, float3 surfaceNormal, float3 viewDirection, float3 worldPosition)
{
	// Calculate light vector and distance
    float3 lightVec = pointLight.position - worldPosition;
    float lightDistance = length(lightVec);
    float3 lightDirection = normalize(lightVec);

    float diffuseFactor = saturate(dot(surfaceNormal, lightDirection));

    // Calculate specular reflection
    float3 specularReflectionDirection = reflect(-lightDirection, surfaceNormal);
    float specularIntensity = pow(saturate(dot(specularReflectionDirection, viewDirection)), puissance);

    // Attenuation = 1 / (Kc + Kl*d + Kq*d²)
    float attenuation = 1.0f / dot(pointLight.attenuation, float3(1.0f, lightDistance, lightDistance * lightDistance));

    // Combine material and light properties
    float3 ambient = pointLight.ambient.rgb * vAMat.rgb;
    float3 diffuse = pointLight.diffuse.rgb * vDMat.rgb * diffuseFactor;
    float3 specular = pointLight.specular.rgb * vSMat.rgb * specularIntensity;

    return (ambient + diffuse + specular) * attenuation;
}

// =====================================
// Main pixel shader
// =====================================

float4 MiniPhongPS(VSOutput input) : SV_Target
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
        float3 texColor = textureEntree.Sample(SampleState, input.textCoord).rgb;
        finalColor *= texColor;
    }

    // Apply underwater effects if the camera is in the water
    if (vCamera.y < WATER_HEIGHT)
    {
        finalColor += ApplyCaustics(input.worldPosition, causticTex, causticSamp, dirLight.direction, elapsedTime);
        finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz);
        finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz);
    }

    return float4(finalColor, 1.0f);
}
