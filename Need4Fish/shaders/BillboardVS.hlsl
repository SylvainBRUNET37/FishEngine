cbuffer BillboardBuffer : register(b0)
{
    float4x4 matWorld;
    float4x4 matView;
    float4x4 matProj;
};

struct VSInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput BillboardVS(VSInput input)
{
    VSOutput output;

    float4 worldPos = mul(float4(input.pos, 1), matWorld);
    float4 viewPos = mul(worldPos, matView);

    output.pos = mul(viewPos, matProj);
    output.uv = input.uv;

    return output;
}
