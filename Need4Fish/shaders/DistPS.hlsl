struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 DistPS(VSOutput input) : SV_Target
{
	// Return a white color
	return float4(1, 1, 1, 1);
}
