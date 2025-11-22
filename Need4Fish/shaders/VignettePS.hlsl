// Harcoded vignette parameters, could be given with CB if needed
// Vignette effect: https://www.youtube.com/watch?v=GiQ5OvDN8dE
static const float VIGNETTE_RADIUS = 0.8; // distance from the center where the effect begins
static const float VIGNETTE_SOFTNESS = 0.5; // transition softness between the zone with and without the effect
static const float VIGNETTE_STRENGTH = 0.3; // intensity of the color (it's like an alpha channel)
static const float4 VINGETTE_COLOR = float4(0.0, 0.0, 0.0, 1.0);

float4 ApplyVignette(float4 textureColor, float2 uv)
{
	const float2 centered = uv * 2.0 - 1.0;
	const float distanceFromCenter = length(centered);

    // Compute how far into the vignette the pixel is, and use that value to darken the image toward the edges
	const float vignetteMask = smoothstep(VIGNETTE_RADIUS, VIGNETTE_RADIUS + VIGNETTE_SOFTNESS, distanceFromCenter);
	const float darkFactor = lerp(1.0, 1.0 - VIGNETTE_STRENGTH, vignetteMask);

    // Tint the vignette with the color
	const float4 tintedColor = lerp(textureColor, VINGETTE_COLOR, vignetteMask * VIGNETTE_STRENGTH);

    // Combine tint + darkening
	const float4 finalColor = float4(tintedColor.rgb * darkFactor, textureColor.a);

	return finalColor;
}