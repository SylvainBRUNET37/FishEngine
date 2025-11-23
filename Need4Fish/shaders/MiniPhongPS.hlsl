// =====================================
// Datas
// =====================================

struct VSOutput
{
    float4 position : SV_Position;
    float3 normal : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
    float2 textCoord : TEXCOORD2;
};

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

// =====================================
// Constant buffers
// =====================================

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

Texture2D sceneTex : register(t0);
SamplerState samp : register(s0);

Texture2D causticTex : register(t1);
SamplerState causticSamp : register(s1);

// =====================================
// Includes
// =====================================

#include "UnderwaterAttenuationPS.hlsl"
#include "UnderwaterFogPS.hlsl"
#include "CausticsPS.hlsl"
#include "BlingPhongPS.hlsl"

// =====================================
// Algorithm
// =====================================

static const float WATER_HEIGHT = 2620.0f; // TODO: do not hardcode if possible

float4 MiniPhongPS(VSOutput input) : SV_Target
{
    float3 finalColor = ApplyBlingPhong(input);

    // Apply underwater effects if the camera is in the water
    if (vCamera.y < WATER_HEIGHT)
    {
        finalColor += ApplyCaustics(input.worldPosition, dirLight.direction, elapsedTime);
        finalColor = ApplyUnderwaterAttenuation(finalColor, input.worldPosition, vCamera.xyz);
        finalColor = ApplyUnderwaterFog(finalColor, input.worldPosition, vCamera.xyz);
    }

    return float4(finalColor, 1.0f);
}
