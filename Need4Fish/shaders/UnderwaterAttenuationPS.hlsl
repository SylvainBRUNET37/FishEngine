float3 ApplyUnderwaterAttenuation(float3 color, float3 worldPos, float3 cameraPos, float waterHeight)
{
    const float3 absorption = float3(0.06f, 0.03f, 0.015f);
    const float waterDensity = 0.020f;

    float pixelDepth = max(0.0f, waterHeight + 10 - worldPos.y);

    // Apply Beer-Lambert attenuation : https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law
    float3 attenuation = exp(-absorption * waterDensity * pixelDepth);

    return color * attenuation;
}

