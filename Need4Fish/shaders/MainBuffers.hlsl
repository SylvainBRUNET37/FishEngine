// =====================================
// Datas
// =====================================

static const float WATER_HEIGHT = 2225.59f; // TODO: do not hardcode if possible

struct VSOutput
{
	float4 position : SV_Position;
	float3 normal : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;
	float2 textCoord : TEXCOORD2;
};

struct DirectionLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 direction;
	float pad; // alignment
};

struct PointLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 position;
	float range; // not used yet but may be useful in the future
	float3 attenuation;
	float pad; // alignment
};

// =====================================
// Constant buffers
// =====================================

#define MAX_POINT_LIGHTS 16

// Per-frame
cbuffer FrameBuffer : register(b0)
{
	float4x4 matViewProj;
	float4 vCamera; // camera pos
	DirectionLight dirLight;

	int pointLightCount;
	float elapsedTime;
	float2 padding_;

	PointLight pointLights[MAX_POINT_LIGHTS];
};

// Per-object
cbuffer ObjectBuffer : register(b1)
{
	float4x4 matWorld;
};

// Per-material
cbuffer MaterialBuffer : register(b2)
{
	float4 vAMat;
	float4 vDMat;
	float4 vSMat;
	float puissance;
	float bTex;
	float2 padding;
};