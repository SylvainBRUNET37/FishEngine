// Per-frame
cbuffer FrameBuffer : register(b0)
{
    float4x4 matViewProj;
    float4 vCamera; // camera pos
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 matWorld;
};

struct VSInput
{
    float3 POS : POSITION;
    float3 NORMAL : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float3 normal : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    float4 worldPos = mul(float4(input.POS, 1.0f), matWorld);

    output.pos = mul(worldPos, matViewProj);
    output.worldPosition = worldPos.xyz;

    float3 normalWorld = mul((float3x3) matWorld, input.NORMAL);
    output.normal = normalize(normalWorld);

    output.uv = input.UV;

    return output;
}