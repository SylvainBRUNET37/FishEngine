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

float2 HeatwaveDistortion(float3 worldPos, float time)
{
    float n = sin(worldPos.x * 0.5 + time * 2.0)
            * cos(worldPos.z * 0.5 + time * 2.0);

    return float2(n, n * 0.5);

    //const float speed = 3.0 + sin(time * 0.7) * 4.5;

    //static const float AMPLITUDE = 0.020;
    //const float wave1 = sin(uv.y * 45.0 + time * speed) * (AMPLITUDE * sin(time * 1.3));
    //const float wave2 = cos(uv.x * 30.0 + time * speed) * (AMPLITUDE * cos(time * 1.6));

    //const float noise = sin((uv.x + uv.y) * 80.0 + time * 5.0) * 0.003;

    //return float2(wave1 + noise, wave2 - noise);
}