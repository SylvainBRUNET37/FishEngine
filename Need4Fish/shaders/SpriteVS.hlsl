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

VSOutput SpriteVS(VSInput input)
{
    VSOutput output;

    output.pos = mul(float4(input.pos, 1.0f), matProj);
    output.uv = input.uv;

    return output;
}
