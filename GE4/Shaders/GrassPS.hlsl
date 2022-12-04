#include "Common.hlsli"
Texture2DArray gLayerMapArray: register(t26);
Texture2D gBlendMap: register(t25);
Texture2D heightmap : register(t24);
TextureCube skymap : register(t10);
Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D roughTex : register(t2);
Texture2D metalTex : register(t3);
Texture2D AOTex : register(t4);
Texture2D emissiveTex : register(t5);
Texture2D<uint2> lightGrid : register(t30);
Texture2D shadowMap : register(t11);
TextureCube shadowMapP : register(t16);
SamplerState sampleWrap : register(s0);
SamplerComparisonState sampleComparison : register(s1);
SamplerState sampleClamp : register(s2);
SamplerState sampleA : register(s3);
SamplerState samHM : register(s4);
float4 main(input_PS_STRAND input) : SV_TARGET
{
	//return input.colour;
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.biNormal = normalize(input.biNormal);
	float4 albedoColor = albedoTex.Sample(sampleWrap, input.uv);
	float3 albedo = albedoColor.rgb;

	float3 normalFromMap = normalTex.Sample(sampleWrap, input.uv).rgb * 2 - 1;

	float3 normal = normalize(CalculateNormalMapping(input.normal, input.tangent, input.biNormal, normalFromMap));

	float roughness = roughTex.Sample(sampleWrap, input.uv).r;

	float metallic = metalTex.Sample(sampleWrap, input.uv).r;

	saturate(metallic);

	float ao = AOTex.Sample(sampleWrap, input.uv).r;




	float3 V = normalize(g_cameraPosition.xyz - input.worldPos);

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);




	float3 lightColor = float3(0, 0, 0);
	float closestDepth = 0;
	float shadow = 1;
	if (g_dirLightOtherConstants.w == SHADOW)
	{
		shadow = calcShadowF(input.shadowPos, sampleComparison, shadowMap);

	}
	float3 Ldir = normalize(g_dirLightDirection.xyz);
	float3 Hdir = normalize(V + Ldir);
	float dDir = 1;
	float3 colorDir = float3(0, 0, 0);

	if (g_booleans.x == 1)
	{
		colorDir = ComputeLightToon(normal, V, F0, Ldir, Hdir, dDir, g_dirLightColor.rgb, metallic, albedo, roughness, g_dirLightOtherConstants.x);
	}
	else
	{
		colorDir = ComputeLight(normal, V, F0, Ldir, Hdir, dDir, g_dirLightColor.rgb, metallic, albedo, roughness, g_dirLightOtherConstants.x);
	}

	lightColor += colorDir;

	float3 ambient = 0.3 * albedo * ao;
	lightColor += ambient;

	// HDR tonemapping
	lightColor = lightColor / (lightColor + float3(1.0, 1.0, 1.0));
	// Gamma correction
	lightColor = pow(lightColor, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));



	lightColor *= shadow;

	float4 finalColor = float4(lightColor,1);
	return finalColor;
}


