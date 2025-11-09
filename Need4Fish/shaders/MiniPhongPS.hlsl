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

#define MAX_POINT_LIGHTS 1

// Per-frame
cbuffer FrameBuffer : register(b0)
{
    float4x4 matViewProj;
    float4 vCamera; // camera position
    DirectionLight dirLight;
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

Texture2D textureEntree : register(t0);
SamplerState SampleState : register(s0);

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
    float3 attenuationFactor = reflect(-lightDirection, surfaceNormal);
    float specularIntensity = pow(saturate(dot(attenuationFactor, viewDirection)), puissance);

    // Attenuation = 1 / (Kc + Kl*d + Kq*d²)
    float attenuation = 1.0f / dot(pointLight.attenuation, float3(1.0f, lightDistance, lightDistance * lightDistance));

    // Combine material and light properties
    float3 ambient = pointLight.ambient.rgb * vAMat.rgb;
    float3 diffuse = pointLight.diffuse.rgb * vDMat.rgb * diffuseFactor;
    float3 specular = pointLight.specular.rgb * vSMat.rgb * specularIntensity;

    return (ambient + diffuse + specular) * attenuation;
}

// =====================================
// Underwater effects
// =====================================

float3 ApplyUnderwaterAttenuation(float3 color, float3 worldPos, float3 cameraPos)
{
    const float3 waterColorAbsorption = float3(0.06f, 0.03f, 0.015f);
    const float waterDensity = 0.020f;

    float distanceFromCamera = length(cameraPos - worldPos);

    const float waterStartDistance = 6.0f; // distance at which the effect start
    const float waterFullDistance = 40.0f; // distance from which the effect is complete

    float depthFactor = saturate((distanceFromCamera - waterStartDistance) / waterFullDistance);

    // Apply Beer-Lambert attenuation : https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law
    float3 attenuation = exp(-waterColorAbsorption * waterDensity * distanceFromCamera * depthFactor);

    return color * attenuation;
}

float3 ApplyUnderwaterFog(float3 color, float3 worldPos, float3 cameraPos)
{
    const float3 fogColor = float3(0.0f, 0.2f, 0.4f);
    const float fogDensity = 0.001f;

    float distanceFromCamera = length(cameraPos - worldPos);

    // Exponential fog : https://rovecoder.net/article/directx-11/fog
    float fogFactor = 1.0f - exp(-pow(fogDensity * distanceFromCamera, 2.0f));
    fogFactor = saturate(fogFactor);

    return lerp(color, fogColor, fogFactor);
}

// TODO: caustics :
// https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-2-rendering-water-caustics

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
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        finalColor += ComputePointLight(pointLights[i], surfaceNormal, viewDirection, input.worldPosition);
    }

    // Apply texture
    if (bTex > 0.5f)
    {
        float3 texColor = textureEntree.Sample(SampleState, input.textCoord).rgb;
        finalColor *= texColor;
    }

    // Apply underwater effects
    if (vCamera.y < 2620) // 2620 = height of the water (TODO: do not hardcode if possible)
    {
        finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz);
        finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz);
    }

    return float4(finalColor, 1.0f);
}
