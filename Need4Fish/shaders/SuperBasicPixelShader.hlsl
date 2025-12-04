struct VSOutput
{
    float4 pos : SV_Position;
    float3 normal : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

Texture2D textureEntree : register(t0);
SamplerState SampleState : register(s0);

float4 main(VSOutput vin) : SV_TARGET
{
    float4 color = textureEntree.Sample(SampleState, vin.uv);

    return color;
}