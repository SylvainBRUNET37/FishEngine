// =====================================
// Constant buffers datas
// =====================================

struct DirLight
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
    DirLight dirLight;
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

float3 ComputeDirLight(float3 N, float3 V)
{
    float3 L = normalize(-dirLight.direction);
    float diff = saturate(dot(N, L));
    float3 R = reflect(-L, N);
    float spec = pow(saturate(dot(R, V)), puissance);

    float3 ambient = dirLight.ambient.rgb * vAMat.rgb;
    float3 diffuse = dirLight.diffuse.rgb * vDMat.rgb * diff;
    float3 specular = dirLight.specular.rgb * vSMat.rgb * spec;

    return ambient + diffuse + specular;
}

float3 ComputePointLight(PointLight L, float3 N, float3 V, float3 P)
{
    float3 lightVec = L.position - P;
    float dist = length(lightVec);
    float3 Lnorm = normalize(lightVec);

    float diff = saturate(dot(N, Lnorm));
    float3 R = reflect(-Lnorm, N);
    float spec = pow(saturate(dot(R, V)), puissance);

    float att = 1.0f / dot(L.attenuation, float3(1.0f, dist, dist * dist));

    float3 ambient = L.ambient.rgb * vAMat.rgb;
    float3 diffuse = L.diffuse.rgb * vDMat.rgb * diff;
    float3 specular = L.specular.rgb * vSMat.rgb * spec;

    return (ambient + diffuse + specular) * att;
}

float4 MiniPhongPS(VSOutput vin) : SV_Target
{
    float3 N = normalize(vin.normal);
    float3 V = normalize(vCamera.xyz - vin.worldPosition);

    float3 result = float3(0, 0, 0);

    // Directional light
    result += ComputeDirLight(N, V);

    // Point lights
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        result += ComputePointLight(pointLights[i], N, V, vin.worldPosition);
    }

    // Texture modulation
    if (bTex > 0.5f)
    {
        float3 texColor = textureEntree.Sample(SampleState, vin.textCoord).rgb;
        result *= texColor;
    }

    return float4(result, 1.0f);
}
