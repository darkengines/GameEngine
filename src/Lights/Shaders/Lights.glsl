float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 computeColor(
	vec3 albedo,
	float roughness,
	float metallic,
	vec3 normal,
	vec3 lightColor,
	vec3 lightDirection,
	float attenuation,
	vec3 viewDirection
) {
	vec3 radiance = attenuation * lightColor.rgb;

	//float diffusion = max(-dot(normal.xyz, lightDirection), 0);
	//vec3 reflectedLightDirection = normalize(reflect(lightDirection, normal.xyz));
	//float reflection = max(-dot(viewDirection, reflectedLightDirection), 0) * specularStrength;

	vec3 halfWay = normalize(lightDirection + viewDirection);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo.xyz, metallic);
	vec3 F = fresnelSchlick(max(dot(halfWay, viewDirection), 0.0), F0);

	float NDF = DistributionGGX(normal.xyz, halfWay, roughness);
	float G = GeometrySmith(normal.xyz, viewDirection, lightDirection, roughness);
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(normal.xyz, viewDirection), 0.0) * max(dot(normal.xyz, lightDirection), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;


	float NdotL = max(dot(normal.xyz, lightDirection), 0.0);
	return (kD * albedo.rgb / PI + specular) * radiance * NdotL;
}