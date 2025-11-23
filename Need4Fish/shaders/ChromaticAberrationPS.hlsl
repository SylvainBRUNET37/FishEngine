static const float2 RED_OFFSET = float2(0.01, 0.0);
static const float2 GREEN_OFFSET = float2(0.02, 0.0);
static const float2 BLUE_OFFSET = float2(-0.01, 0.0);

float4 ComputeChromaticAberration(Texture2D sceneTex, SamplerState samp, float2 uv)
{
	float r = sceneTex.Sample(samp, uv + RED_OFFSET).r;
	float g = sceneTex.Sample(samp, uv + GREEN_OFFSET).g;
	float b = sceneTex.Sample(samp, uv + BLUE_OFFSET).b;

	return float4(r, g, b, 1.0);
}
