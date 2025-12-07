float3 ComputeDirLight(float3 surfaceNormal, float3 viewDirection)
{
	// Calculate light direction
	float3 lightDirection = normalize(-dirLight.direction);
	float diffuseIntensity = saturate(dot(surfaceNormal, lightDirection));

    // Calculate specular reflection
	float3 reflectionDirection = reflect(-lightDirection, surfaceNormal);
	float specularIntensity = pow(saturate(dot(reflectionDirection, viewDirection)), puissance);

    // Combine material and light properties
	float3 ambient = dirLight.ambient.rgb * vAMat.rgb;
	float3 diffuse = dirLight.diffuse.rgb * vDMat.rgb * diffuseIntensity;
	float3 specular = dirLight.specular.rgb * vSMat.rgb * specularIntensity;

	return ambient + diffuse + specular;
}

float3 ComputeDirLight(float3 surfaceNormal, float3 viewDirection, float shadow)
{
	// Calculate light direction
    float3 lightDirection = normalize(-dirLight.direction);
    float diffuseIntensity = saturate(dot(surfaceNormal, lightDirection));

    // Calculate specular reflection
    float3 reflectionDirection = reflect(-lightDirection, surfaceNormal);
    float specularIntensity = pow(saturate(dot(reflectionDirection, viewDirection)), puissance);

    // Combine material, light and shadow properties
    float3 ambient = dirLight.ambient.rgb * vAMat.rgb;
    float3 diffuse = dirLight.diffuse.rgb * vDMat.rgb * diffuseIntensity * shadow;
    float3 specular = dirLight.specular.rgb * vSMat.rgb * specularIntensity * shadow;

    return ambient + diffuse + specular;
}

float3 ComputePointLight(PointLight pointLight, float3 surfaceNormal, float3 viewDirection, float3 worldPosition)
{
	// Calculate light vector and distance
	float3 lightVec = pointLight.position - worldPosition;
	float lightDistance = length(lightVec);
	float3 lightDirection = normalize(lightVec);

	float diffuseFactor = saturate(dot(surfaceNormal, lightDirection));

    // Calculate specular reflection
	float3 specularReflectionDirection = reflect(-lightDirection, surfaceNormal);
	float specularIntensity = pow(saturate(dot(specularReflectionDirection, viewDirection)), puissance);

    // Attenuation = 1 / (Kc + Kl*d + Kq*d²)
	float attenuation = 1.0f / dot(pointLight.attenuation, float3(1.0f, lightDistance, lightDistance * lightDistance));

    // Combine material and light properties
	float3 ambient = pointLight.ambient.rgb * vAMat.rgb;
	float3 diffuse = pointLight.diffuse.rgb * vDMat.rgb * diffuseFactor;
	float3 specular = pointLight.specular.rgb * vSMat.rgb * specularIntensity;

	return (ambient + diffuse + specular) * attenuation;
}