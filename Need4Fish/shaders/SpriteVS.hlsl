// =====================================
// Constant buffers datas
// =====================================

cbuffer MatrixBuffer : register(b0)
{
    matrix matProj;
};

// =====================================
// Inputs / Outputs
// =====================================

struct VSInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// =====================================
// Algorithm
// =====================================

VSOutput SpriteVS(VSInput vin) // rename it to SpriteVS
{
    VSOutput output;

    output.pos = mul(float4(vin.pos, 1.0f), matProj);
    output.uv = vin.uv;

    return output;
}
