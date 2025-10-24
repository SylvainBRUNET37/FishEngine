cbuffer param : register(b0)
{
    float4x4 matWorldViewProj;
    float4x4 matWorld;
    float4 vLumiere;
    float4 vCamera;
    float4 vAEcl;
    float4 vAMat;
    float4 vDEcl;
    float4 vDMat;
    float4 vSEcl;
    float4 vSMat;
    float puissance;
    int bTex;
    float2 remplissage;
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

    vout.Pos = mul(float4(vin.POSITION, 1.0f), matWorldViewProj);
    vout.Norm = mul(float4(vin.NORMAL, 0.0f), matWorld).xyz;

    float3 posWorld = mul(float4(vin.POSITION, 1.0f), matWorld).xyz;
    vout.vDirLum = vLumiere.xyz - posWorld;
    vout.vDirCam = vCamera.xyz - posWorld;
    vout.coordTex = vin.TEXCOORD;

    return vout;
}
