
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



PS_OUTPUT main(PS_INPUT_TERRAIN input) : SV_TARGET
{
	PS_OUTPUT output;

	float g_texelCellU = 1.0f / (float)HM_W;
	float g_texelCellV = 1.0f / (float)HM_H;
	float g_worldCell = (float)CELL_SPACING;

	//doing this in ps and not in ds to remove shimmering
	float2 leftTex = input.uv + float2(-g_texelCellU, 0.0f);
	float2 rightTex = input.uv + float2(g_texelCellU, 0.0f);
	float2 bottomTex = input.uv + float2(0.0f, g_texelCellV);
	float2 topTex = input.uv + float2(0.0f, -g_texelCellV);

	float leftY = heightmap.SampleLevel(samHM, leftTex, 0).r;
	float rightY = heightmap.SampleLevel(samHM, rightTex, 0).r;
	float bottomY = heightmap.SampleLevel(samHM, bottomTex, 0).r;
	float topY = heightmap.SampleLevel(samHM, topTex, 0).r;

	float3 tangent = normalize(float3(2.0f * g_worldCell, rightY - leftY, 0.0f));
	float3 biNormal = normalize(float3(0.0f, bottomY - topY, -2.0f * g_worldCell));
	float3 normal = cross(tangent, biNormal);
	
	

	float4 c0 = gLayerMapArray.Sample(sampleWrap, float3(input.tileUV, 0.0f));
	float4 c1 = gLayerMapArray.Sample(sampleWrap, float3(input.tileUV, 1.0f));
	float4 c2 = gLayerMapArray.Sample(sampleWrap, float3(input.tileUV, 2.0f));
	

	// Sample the blend map.
	float4 t = gBlendMap.Sample(sampleWrap, input.uv);

	// Blend the layers on top of each other.
	float4 texColor = c0;
	texColor = lerp(texColor, c1, t.r);
	texColor = lerp(texColor, c2, t.g);

	
	float4 albedoColor = texColor;
	float3 albedo = texColor.rgb;

	float3 normalFromMap = normalTex.Sample(sampleWrap, input.uv).rgb * 2 - 1;

	//float3 normal = normalize(CalculateNormalMapping(input.normal, input.tangent, input.biNormal, normalFromMap));

	float roughness = roughTex.Sample(sampleWrap, input.uv).r;

	float metallic = metalTex.Sample(sampleWrap, input.uv).r;

	saturate(metallic);

	float ao = AOTex.Sample(sampleWrap, input.uv).r;




	float3 V = normalize(g_cameraPosition.xyz - input.worldPos);
	V /= length(V);
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);


	uint2 tileIndex = uint2(floor(input.pos.xy / BLOCK_SIZE));

	uint startOffset = lightGrid[tileIndex].x;
	uint lightCount = lightGrid[tileIndex].y;



	float3 lightColor = float3(0, 0, 0);
	float closestDepth = 0;
	float shadow = 1;
	float pointShadow = 1;
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
	[unroll(MAX_LIGHTS)]
	for (uint i = 0; i < lightCount; i++)
	{
		uint lightIndex = SB_LIGHT_INDEX[startOffset + i];
		Light light = SB_LIGHT[lightIndex];
		float3 lightDir = light.direction.xyz;
		switch (light.type.x)
		{
		case POINT_LIGHT:

		{
			float3 L = normalize(light.position.xyz - input.worldPos);
			float3 H = normalize(V + L);
			float d = length(light.position.xyz - input.worldPos.xyz);


			float3 color = float3(0, 0, 0);

			if (g_booleans.x == 1)
			{
				color = ComputeLightToon(normal, V, F0, L, H, d, light.color.rgb, metallic, albedo, roughness, light.otherConstants.x);
			}
			else
			{
				color = ComputeLight(normal, V, F0, L, H, d, light.color.rgb, metallic, albedo, roughness, light.otherConstants.x);
			}
			if (light.otherConstants.w == SHADOW)
			{
				float3 fragToLight = (input.worldPos - light.position.xyz);
				pointShadow = calcShadowF(fragToLight, sampleComparison, shadowMapP, sampleClamp, V);
				pointShadow = 1 - pointShadow;
				
			}


			lightColor += color;
			break;
		}





		case SPOT_LIGHT:
		{
			float3 fragLightDir = input.worldPos - light.position.xyz;
			float theta = dot(-fragLightDir, normalize(-lightDir));

			if (theta > light.otherConstants.z)
			{
				lightDir = normalize(lightDir);

				float3 L = normalize(light.position.xyz - input.worldPos);
				float3 H = normalize(V + L);
				float d = length(light.position.xyz - input.worldPos.xyz);

				float3 color = float3(0, 0, 0);
				if (g_booleans.x == 1)
				{
					color = ComputeLightToon(normal, V, F0, L, H, d, light.color.rgb, metallic, albedo, roughness, light.otherConstants.x);
				}
				else
				{
					color = ComputeLight(normal, V, F0, L, H, d, light.color.rgb, metallic, albedo, roughness, light.otherConstants.x);
				}




				lightColor += color;
			}
			break;
		}

		}


	}





	float3 ambient = 0.3 * albedo * ao;
	lightColor += ambient;

	// HDR tonemapping
	lightColor = lightColor / (lightColor + float3(1.0, 1.0, 1.0));
	// Gamma correction
	lightColor = pow(lightColor, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));


	lightColor *= shadow;
	lightColor *= pointShadow;

	float4 finalColor = float4(lightColor, albedoColor.a);


	float3 n = mul(float4(normal, 1), g_view).xyz;

	output.worldPos = float4(input.worldPos, 1);
	output.color = saturate(finalColor);
	output.normalView = float4(normalize(n), input.posView.z);
	output.emissive = emissiveTex.Sample(sampleWrap, input.uv);
	output.emissive.rgb *= 3;
	return output;

}