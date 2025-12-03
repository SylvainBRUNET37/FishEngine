#include "UnderwaterFogPS.hlsl"
#include "UnderwaterAttenuationPS.hlsl"

cbuffer BillboardBuffer : register(b0)
{
	float4x4 matWorld;
	float4x4 matView;
	float4x4 matProj;
	float3 cameraPos;
	float pad;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 BubblePS(VSOutput input) : SV_Target
{
	float4 finalColor = tex.Sample(samp, input.uv);

    // Apply fog & attenuation
	finalColor.rgb = ApplyUnderwaterAttenuation(finalColor, input.pos, cameraPos);
	finalColor.rgb = ApplyUnderwaterFog(finalColor, input.pos, cameraPos);

	return finalColor;
}
