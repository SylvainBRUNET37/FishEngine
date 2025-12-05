//Constant buffer data
cbuffer cbPerFrame : register(b0)
{
    float4x4 gLightWVP; //WVP for World View Projection, probably
};

// Nonnumeric values cannot be added to a cbuffer
Texture2D gDiffuseMap : register(t0);

SamplerState samLinear : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AdressU = Wrap;
    AdressV = Wrap;
};

struct VertexIn
{
    float3 PosL : POSITION;
    //float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 Profondeur : TEXCOORD1;
};

void main(VertexOut pin)
{
    float4 diffuse = gDiffuseMap.Sample(samLinear, pin.Tex);
    clip(diffuse.a - 0.15f);
}