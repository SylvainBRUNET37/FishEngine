struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 BillboardPS(VSOutput input) : SV_Target
{
    float4 finalColor = tex.Sample(samp, input.uv);

    // Discard transparents pixels
    clip(finalColor.a - 0.01f);

    return finalColor;
}
