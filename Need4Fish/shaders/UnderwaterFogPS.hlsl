float3 ApplyUnderwaterFog(float3 color, float3 worldPos, float3 cameraPos)
{
	const float3 fogColor = float3(0.0f, 0.2f, 0.4f);
	const float fogDensity = 0.0006f;

	float distanceFromCamera = length(cameraPos - worldPos);

    // Exponential fog:
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/fog-formulas
    // https://rovecoder.net/article/directx-11/fog
	float fogFactor = 1.0f - exp(-pow(fogDensity * distanceFromCamera, 2.0f));
	fogFactor = saturate(fogFactor);

	return lerp(color, fogColor, fogFactor);
}
