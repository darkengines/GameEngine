// GGX Normal Distribution Function
float computeDistributionGGX(vec3 normal, vec3 halfVector, float roughness) {
	float alpha = roughness * roughness;
	float alphaSquared = alpha * alpha;
	float nDotH = max(dot(normal, halfVector), 0.0);
	float nDotHSquared = nDotH * nDotH;

	float numerator = alphaSquared;
	float denominator = (nDotHSquared * (alphaSquared - 1.0) + 1.0);
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

// Schlick-GGX Geometry Function (Single)
float computeGeometrySchlickGGX(float nDotV, float roughness) {
	float k = pow(roughness + 1.0, 2.0) / 8.0;

	float numerator = nDotV;
	float denominator = nDotV * (1.0 - k) + k;

	return numerator / denominator;
}

// Smith's Geometry Function
float computeGeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
	float nDotV = max(dot(normal, viewDir), 0.0);
	float nDotL = max(dot(normal, lightDir), 0.0);
	float ggx2 = computeGeometrySchlickGGX(nDotV, roughness);
	float ggx1 = computeGeometrySchlickGGX(nDotL, roughness);

	return ggx1 * ggx2;
}

// Fresnel-Schlick Approximation
vec3 computeFresnelSchlick(float cosTheta, vec3 f0) { return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0); }

// Compute the final color using the Metallic workflow
vec3 computeMetallicColor(vec3 albedo, float roughness, float metallic, vec3 normal, vec3 lightColor, vec3 lightDir, float attenuation, vec3 viewDir) {
	vec3 radiance = attenuation * lightColor;
	vec3 halfVector = normalize(lightDir + viewDir);

	// Reflectance at normal incidence
	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	// Fresnel-Schlick approximation for specular reflectance
	vec3 fresnel = computeFresnelSchlick(max(dot(halfVector, viewDir), 0.0), f0);

	// Geometry and Normal Distribution Function
	float ndf = computeDistributionGGX(normal, halfVector, roughness);
	float geometry = computeGeometrySmith(normal, viewDir, lightDir, roughness);

	// Specular component
	vec3 specular = (ndf * geometry * fresnel) / max(4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0), 0.001);

	// Energy conservation
	vec3 ks = fresnel;
	vec3 kd = vec3(1.0) - ks;
	kd *= 1.0 - metallic;

	// Diffuse and final color
	float nDotL = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = kd * albedo / PI;

	// Final color mix
	return (diffuse + specular) * radiance * nDotL;
}

// Compute the final color using the Specular workflow
vec3 computeSpecularColor(vec3 albedo, float roughness, vec3 specularColor, vec3 normal, vec3 lightColor, vec3 lightDir, float attenuation, vec3 viewDir) {
	vec3 radiance = attenuation * lightColor;
	vec3 halfVector = normalize(lightDir + viewDir);

	// Reflectance at normal incidence
	vec3 f0 = specularColor;

	// Fresnel-Schlick approximation for specular reflectance
	vec3 fresnel = computeFresnelSchlick(max(dot(halfVector, viewDir), 0.0), f0);

	// Geometry and Normal Distribution Function
	float ndf = computeDistributionGGX(normal, halfVector, roughness);
	float geometry = computeGeometrySmith(normal, viewDir, lightDir, roughness);

	// Specular component
	vec3 specular = (ndf * geometry * fresnel) / max(4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0), 0.001);

	// Energy conservation
	vec3 ks = fresnel;
	vec3 kd = vec3(1.0) - ks;

	// Diffuse and final color
	float nDotL = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = kd * albedo / PI;

	// Final color mix
	return (diffuse + specular) * radiance * nDotL;
}
