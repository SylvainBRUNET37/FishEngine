// =====================================
// Datas
// =====================================

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// =====================================
// Algorithm
// =====================================

Texture2D textureEntree : register(t0);
SamplerState SampleState : register(s0);

float4 SpritePS(VSOutput vin) : SV_Target 
{
    float4 color = textureEntree.Sample(SampleState, vin.uv);

    return color;
}
