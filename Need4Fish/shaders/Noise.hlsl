// https://thebookofshaders.com/
// https://thebookofshaders.com/13

// Random float between 0 and 1 based on UV
float Random(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453123);
}

// https://gamedev.stackexchange.com/questions/198247/creating-large-perlin-noise-spaces-on-gpu
float ApplyNoise(float2 p)
{
	float2 i = floor(p);
	float2 f = frac(p);

	float a = Random(i);
	float b = Random(i + float2(1.0, 0.0));
	float c = Random(i + float2(0.0, 1.0));
	float d = Random(i + float2(1.0, 1.0));

	float2 u = f * f * (3.0 - 2.0 * f);

	return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

float2 ApplyDistortion(float2 uv)
{
	float noiseX = ApplyNoise(uv);
	float noiseY = ApplyNoise(uv);
	
	static const float STRENGTH = 0.1;
	return uv + float2(noiseX, noiseY) * STRENGTH;
}

static const float3 NOISE_FREQUENCE = float3(3.0, 6.0, 9.0);
static const float3 NOISE_SPEED = float3(0.5, -1.0, 1.5);
static const float3 NOISE_AMPLITUDE = float3(0.45, 0.30, 0.15);

float CreateNoise(float2 uv, float time)
{
	float2 distordedUV = ApplyDistortion(uv * 2.0);

	// Add noise with 3 octaves
	return ApplyNoise(distordedUV * NOISE_FREQUENCE.x + time * NOISE_SPEED.x) * NOISE_AMPLITUDE.x +
	    ApplyNoise(distordedUV * NOISE_FREQUENCE.y + time * NOISE_SPEED.y) * NOISE_AMPLITUDE.y +
	    ApplyNoise(distordedUV * NOISE_FREQUENCE.z + time * NOISE_SPEED.z) * NOISE_AMPLITUDE.z;
}

