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

struct VSInput
{
    float3 POSITION : POSITION;
    float3 NORMAL : NORMAL;
    float2 TEXCOORD : TEXCOORD;
};

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

VSOutput MiniPhongVS(VSInput input)
{
    VSOutput vout = (VSOutput) 0;

    float4 worldPos = mul(float4(input.POSITION, 1.0f), matWorld);

    vout.position = mul(worldPos, matViewProj);
    vout.worldPosition = worldPos.xyz;

    float3 normalWorld = mul((float3x3) matWorld, input.NORMAL);
    vout.normal = normalize(normalWorld);

    vout.textCoord = input.TEXCOORD;

    return vout;
}
