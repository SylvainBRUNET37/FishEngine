cbuffer BillboardCameraBuffer : register(b0)
{
	float4x4 matView;
	float4x4 matProj;
	float3 cameraPos;
	float pad;
};

StructuredBuffer<float4x4> BillboardWorlds : register(t1);

struct VSInput
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	uint instanceID : SV_InstanceID;
};

struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
};

VSOutput BubbleInstancingVS(VSInput input)
{
	VSOutput output;

	float4x4 matWorld = BillboardWorlds[input.instanceID];

	float4 worldPos = mul(float4(input.pos, 1.0f), matWorld);
	float4 viewPos = mul(worldPos, matView);
	output.pos = mul(viewPos, matProj);

	output.uv = input.uv;
	output.worldPos = worldPos.xyz;

	return output;
}
