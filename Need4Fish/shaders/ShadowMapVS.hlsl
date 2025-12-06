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

VertexOut main(VertexIn vIn)
{
    VertexOut vOut;
        
    vOut.PosH = mul(float4(vIn.PosL, 1.0f), gLightWVP); //reference uses gWorldViewProj here despite not having it? //Atelier says to use WVP of light also...
    //reference material uses gTexTransform despite not having it...
    //vOut.Tex = mul(float4(vIn.Tex, 0.0f, 1.0f), gTexTransform).xy;
    vOut.Profondeur.x = vOut.PosH.z / vOut.PosH.w;
    vOut.Tex = vIn.Tex;
    
    return vOut;
}