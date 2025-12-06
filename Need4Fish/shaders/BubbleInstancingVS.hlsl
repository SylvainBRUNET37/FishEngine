cbuffer BillboardCameraBuffer : register(b0)
{
	float4x4 matView;
	float4x4 matProj;
	float3 cameraPos;
	float pad;
};

struct BillboardData
{
    float3 position;
    float scale;
};

StructuredBuffer<BillboardData> BillboardWorlds : register(t1);

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

	// Get billboard data
    BillboardData data = BillboardWorlds[input.instanceID];
    float3 billboardPos = data.position.xyz;
    float2 billboardScale = data.scale.xx;

	// Compute orientation vectprs
    float3 forward = normalize(cameraPos - billboardPos);
    float3 worldUp = float3(0, 1, 0);
    float3 right = normalize(cross(worldUp, forward));
    float3 up = cross(forward, right);

	// Scale
    float3 local = float3
	(
		input.pos.x * billboardScale.x,
		input.pos.y * billboardScale.y,
		0.0f
	);

	// Position
    float3 worldPos = billboardPos +
                      right * local.x +
                      up * local.y;

	// Apply view & proj
    float4 viewPos = mul(float4(worldPos, 1.0f), matView);
    output.pos = mul(viewPos, matProj);

    output.uv = input.uv;
    output.worldPos = worldPos;

	return output;
}
