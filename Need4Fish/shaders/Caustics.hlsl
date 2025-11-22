// https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-2-rendering-water-caustics
// https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-1-effective-water-simulation-physical-models

static const float VTXSIZE = 0.15f;
static const float WAVESIZE = 0.8f;
static const float FACTOR = 0.5f;
static const float SPEED = 2.0f;
static const int OCTAVES = 4;

static const float AIR_INDICE_OF_REFRACTION = 1.0f;
static const float WATER_INDICE_OF_REFRACTION = 1.3f;

float GetWaveHeight(float x, float y, float elapsedTime)
{
	float z = 0.0f;
	float octaves = OCTAVES;
	float factor = FACTOR;
	float d = sqrt(x * x + y * y);

	do
	{
		float angle = elapsedTime * SPEED + (1.0f / factor) * x * y * WAVESIZE;
		z -= factor * cos(angle);

		factor *= 0.5f; // divide by 2
		octaves--;
	}
    while (octaves > 0);

	return 2.0f * VTXSIZE * d * z;
}

float2 ComputeGradWave(float2 worldPosXZ, float elapsedTime)
{
    // Make wave largers
	worldPosXZ *= 0.01;

	float2 gradiant = 0;
	float amplitude = FACTOR;
	float frequency = WAVESIZE;
	float timer = elapsedTime * SPEED;

    // Eech octave is a wave pattern, sum all of them to have a "wave noise"
	for (int i = 0; i < OCTAVES; i++)
	{
    	// For a better look and to avoid repetitive patterns
		static const float GOLDEN_ANGLE = 2.39996;

        // Direction of the wave movement
		float2 waveDirection = normalize(float2
        (
            cos(i * GOLDEN_ANGLE),
            sin(i * GOLDEN_ANGLE)
        ));

        // Position projected onto direction + time
		float wavePhase = dot(worldPosXZ, waveDirection) * frequency + timer;

        // Derivative of the sine wave
		float sinWavePhase = cos(wavePhase);

        // Add the gradient contribution from this octave
		gradiant += amplitude * sinWavePhase * (frequency * waveDirection);

    	// Reduce amplitude and increase frequency for the next octave
		amplitude *= 0.5;
		frequency *= 2.0;
	}

	return gradiant * VTXSIZE;
}

float3 GetWaveNormal(float2 worldPosXZ, float elapsedTime)
{
	float2 gradWave = ComputeGradWave(worldPosXZ, elapsedTime);

    // Convert to: (-dZ/dx, 1, -dZ/dy)
	float3 waveNormal = normalize(float3(-gradWave.x, 1.0f, -gradWave.y));

	return waveNormal;
}

float ApplyCaustics(
	float3 worldPos, 
	Texture2D causticsTex, 
	SamplerState causticSamp,
	float3 dirLightDirection, 
	float elapsedTime
)
{
	// Horizontal positon of the ocean
	const float2 surfXZ = worldPos.xz;

    // Get the water surface height and normal
	float waterY = GetWaveHeight(surfXZ.x, surfXZ.y, elapsedTime);
	float3 waveNormal = GetWaveNormal(surfXZ, elapsedTime);

    // Raycast which goes toward the sky
	const float3 UP_RAY = float3(0, 1, 0);

    // Refract the ray into air
    // Snell law: https://en.wikipedia.org/wiki/Snell%27s_law
	const float ETA = AIR_INDICE_OF_REFRACTION / WATER_INDICE_OF_REFRACTION;
	float3 refracted = refract(-UP_RAY, waveNormal, ETA);

    // Sun direction
	const float3 sunDir = normalize(dirLightDirection);

    // Compute the alignement/difference between the reracted ray and the sun dir
	float alignment = saturate(dot(normalize(refracted), sunDir));

    // Add intensity to the caustic effect
	const float STRENGTH = 8.0;
	float intensity = pow(alignment, STRENGTH);

    // Apply caustic texture patterns
	float depth = waterY - worldPos.y;
	float3 proj = worldPos + refracted * depth;

	const float UV_SCALE = 0.005;
	float causticTexture = causticsTex.Sample(causticSamp, proj.xz * UV_SCALE).r;

	return intensity * causticTexture;
}