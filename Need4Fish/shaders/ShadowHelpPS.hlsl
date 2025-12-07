static const float SHADOW_MAP_SIZE = 4096.0f; //Shadow map size must also be defined in RenderSystem
static const float SHADOW_MAP_DX = 1.0f / SHADOW_MAP_SIZE;
float CalcShadowFactor(SamplerComparisonState samShadow, Texture2D shadowMap, float4 shadowPosH)
{
	//Complete projection by doing division by w
    shadowPosH.xyz /= shadowPosH.w;
	
	//depth in NDC space
    float depth = shadowPosH.z;
	
	// Texel size
    const float dx = SHADOW_MAP_DX;
	
    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };
	
	//3x3 box filter pattern. Each sample does a 4-tap PCF.
	[unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
		shadowPosH.xy + offsets[i], depth).r;
    }
	
	// Average the samples.
    return percentLit /= 9.0f;
}