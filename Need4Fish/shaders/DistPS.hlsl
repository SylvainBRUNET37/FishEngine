struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 DistPS(VSOutput input) : SV_Target
{
	// Return a green color
	return float4(0, 1, 0, 1);
}
