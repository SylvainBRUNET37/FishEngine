float3 ApplyUnderwaterAttenuation(float3 color, float3 worldPos, float3 cameraPos)
{
	const float3 waterColorAbsorption = float3(0.06f, 0.03f, 0.015f);
	const float waterDensity = 0.020f;

	float distanceFromCamera = length(cameraPos - worldPos);

	const float waterStartDistance = 6.0f; // distance at which the effect start
	const float waterFullDistance = 40.0f; // distance from which the effect is complete

	float depthFactor = saturate((distanceFromCamera - waterStartDistance) / waterFullDistance);

	// Apply Beer-Lambert attenuation : https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law
	float3 attenuation = exp(-waterColorAbsorption * waterDensity * distanceFromCamera * depthFactor);

	return color * attenuation;
}
