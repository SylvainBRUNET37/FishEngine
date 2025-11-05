// Per-frame: Camera and light data
cbuffer FrameBuffer : register(b0)
{
    float4x4 matViewProj;
    float4 vLumiere; // xyz = light position
    float4 vCamera; // xyz = camera position
    float4 vAEcl; // ambient light color
    float4 vDEcl; // diffuse light color
    float4 vSEcl; // specular light color
};

// Per-object: Transform
cbuffer ObjectBuffer : register(b1)
{
    float4x4 matWorld;
};

// Per-material: Material parameters
cbuffer MaterialBuffer : register(b2)
{
    float4 vAMat; // ambient color
    float4 vDMat; // diffuse color
    float4 vSMat; // specular color
    float puissance; // shininess
    float bTex; // texture enabled (float for alignment)
    float2 padding; // alignment padding
};

struct VSInput
{
    float3 POSITION : POSITION;
    float3 NORMAL : NORMAL;
    float2 TEXCOORD : TEXCOORD;
};

struct VSOutput
{
    float4 Pos : SV_Position;
    float3 Norm : TEXCOORD0;
    float3 vDirLum : TEXCOORD1;
    float3 vDirCam : TEXCOORD2;
    float2 coordTex : TEXCOORD3;
};

VSOutput MiniPhongVS(VSInput vin)
{
    VSOutput vout = (VSOutput) 0;

    // Transform position and normal
    float4 worldPos = mul(float4(vin.POSITION, 1.0f), matWorld);
    vout.Pos = mul(worldPos, matViewProj);

    vout.Norm = normalize(mul(float4(vin.NORMAL, 0.0f), matWorld).xyz);

    // Compute direction vectors (world space)
    vout.vDirLum = vLumiere.xyz - worldPos.xyz;
    vout.vDirCam = vCamera.xyz - worldPos.xyz;

    vout.coordTex = vin.TEXCOORD;
    return vout;
}
