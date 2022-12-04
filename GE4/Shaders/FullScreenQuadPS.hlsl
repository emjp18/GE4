#include "Common.hlsli"

Texture2D albedoQuad : register(t10);
Texture2D depthTexture : register(t0);
Texture2D emissiveTex : register(t5);
Texture2D<uint2> lightGrid : register(t30);
Texture2D shadowMap : register(t11);
TextureCube shadowMapP : register(t16);
SamplerState sampleWrap : register(s0);
SamplerComparisonState sampleComparison : register(s1);
SamplerState sampleClamp : register(s2);
SamplerState sampleA : register(s3);
SamplerState samHM : register(s4);
Texture2D emissiveQuad: register(t33);
Texture2D frameQuad : register(t34);
Texture2D volLightQuad : register(t12);
Texture2D BilateralBlurHorQuad : register(t13);
Texture2D gNormalDepthMap : register(t20);
Texture2D posTexture: register (t21);

float gWeights[11] =
{
	0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
};
static const int gBlurRadius = 5;
static const float4 kRadarColors[14] =
{
    {0,0.9255,0.9255,1},   // cyan
    {0,0.62745,0.9647,1},  // light blue
    {0,0,0.9647,1},        // blue
    {0,1,0,1},             // bright green
    {0,0.7843,0,1},        // green
    {0,0.5647,0,1},        // dark green
    {1,1,0,1},             // yellow
    {0.90588,0.75294,0,1}, // yellow-orange
    {1,0.5647,0,1},        // orange
    {1,0,0,1},             // bright red
    {0.8392,0,0,1},        // red
    {0.75294,0,0,1},       // dark red
    {1,0,1,1},             // magenta
    {0.6,0.3333,0.7882,1}, // purple
};


float4 main(psQuadInput Input) : SV_TARGET
{
	return albedoQuad.Sample(sampleWrap, Input.uv);
    
    


}
float4 debugLightCulling(psQuadInput Input) : SV_TARGET
{

    uint2 tileIndex = uint2(abs(floor(Input.pos.xy / ((float)BLOCK_SIZE))));
    uint nNumLightsInThisTile = lightGrid[tileIndex].y;

	nNumLightsInThisTile += 1;
    // black for no lights
    if (nNumLightsInThisTile == 0) return float4(0,0,0,1);
    // light purple for reaching the max
    else if (nNumLightsInThisTile == 200) return float4(0.847,0.745,0.921,1);
    // white for going over the max
    else if (nNumLightsInThisTile > 200) return float4(1,1,1,1);
    // else use weather radar colors
    else
    {

        // use a log scale to provide more detail when the number of lights is smaller

        // want to find the base b such that the logb of g_uMaxNumLightsPerTile is 14
        // (because we have 14 radar colors)
        float fLogBase = exp2(0.07142857f * log2((float)200));

        // change of base
        // logb(x) = log2(x) / log2(b)
        uint nColorIndex = floor(log2((float)nNumLightsInThisTile) / log2(fLogBase));
        return kRadarColors[nColorIndex];
    }

}
float4 add(psQuadInput Input) : SV_TARGET
{
   
    return saturate(emissiveQuad.Sample(sampleWrap, Input.uv)+ (frameQuad.Sample(sampleWrap, Input.uv))+ volLightQuad.Sample(sampleWrap, Input.uv));




}

float3 WorldPosFromDepth(float depth, float2 uv) {
	float z = depth * 2.0 - 1.0;

	float4 clipSpacePosition = float4(uv * 2.0 - 1.0, z, 1.0);
	float4 viewSpacePosition = mul(clipSpacePosition, g_inverseProjection);

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	float4 worldSpacePosition = mul(viewSpacePosition, g_inverseView);

	return worldSpacePosition.xyz;
}

float ComputeScattering(float lightDotView)
{
    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * lightDotView, 1.5f));
    return result;
}
static const float ditherPattern[4][4] = { { 0.0f, 0.5f, 0.125f, 0.625f},
{ 0.75f, 0.22f, 0.875f, 0.375f},
{ 0.1875f, 0.6875f, 0.0625f, 0.5625},
{ 0.9375f, 0.4375f, 0.8125f, 0.3125} };

float4 rayMarch(psQuadInput input, uint samIndex : SV_SampleIndex) : SV_TARGET
{

	/*uint2 xy = GetRenderTargetSamplePosition(samIndex);

	int xmod = int(fmod(xy.x, 4.0));
	int ymod = int(fmod(xy.y, 4.0));

	float dither = ditherPattern[ymod][xmod];*/

	/*float3 worldPos = posTexture.Load(float3(xy,0)).xyz;*/

	float depth = depthTexture.Sample(sampleWrap, input.uv).r;

	float3 worldPos = WorldPosFromDepth(depth, input.uv);

	//float3 worldPos = posTexture.Sample(sampleWrap, input.uv).xyz;

	float3 startPosition = g_cameraPosition.xyz;

	float3 rayVector = worldPos - startPosition;

	float rayLength = length(rayVector);
	float3 rayDirection = rayVector / rayLength;

	float stepLength = rayLength / NB_STEPS;

	float3 step = rayDirection * stepLength;

	float3 currentPosition = startPosition;

	float3 accumFog = 0.0f.xxx;


	for (int i = 0; i < NB_STEPS; i++)
	{

		float4 worldInShadowCameraSpace = mul(float4(currentPosition, 1.0f), g_shadowVP);

		worldInShadowCameraSpace /= worldInShadowCameraSpace.w;
		float2 uv = (worldInShadowCameraSpace.xy + float2(1.0, 1.0)) * 0.5;
		uv = float2(uv.x, 1 - uv.y);
		float shadowMapValue = shadowMap.SampleLevel(sampleClamp, uv, 0).r;


		if (shadowMapValue > worldInShadowCameraSpace.z)
		{

			accumFog += ComputeScattering(dot(rayDirection, g_dirLightDirection.xyz)).xxx;
			/*float3 noise_pos = (currentPosition + 10 * g_time.x) * 10;
			float noise = lerp(1.0, saturate(cnoise(noise_pos)), 0.5);
			accumFog *= noise;*/
		}
		int xmod = int(fmod(i, 4.0));
		int ymod = int(fmod(i+1, 4.0));
		currentPosition += step* ditherPattern[xmod][ymod];


	}

	accumFog *= g_dirLightColor.xyz;

	accumFog /= NB_STEPS;

	

	return float4(accumFog,1);



}


float4 bilateralBlurHor(psQuadInput pin) : SV_Target
{
	
	float2 texOffset = float2(g_screenDimenstions.z, 0.0f);

	// The center value always contributes to the sum.
	float4 color = gWeights[5] * volLightQuad.SampleLevel(samHM, pin.uv, 0.0);
	float totalWeight = gWeights[5];

	float4 centerNormalDepth = gNormalDepthMap.SampleLevel(samHM, pin.uv, 0.0f);

	for (float i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		// We already added in the center weight.
		if (i == 0)
			continue;

		float2 tex = pin.uv + i * texOffset;

		float4 neighborNormalDepth = gNormalDepthMap.SampleLevel(
			samHM, tex, 0.0f);

		//
		// If the center value and neighbor values differ too much (either in 
		// normal or depth), then we assume we are sampling across a discontinuity.
		// We discard such samples from the blur.
		//

		if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f &&
			abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
		{
			float weight = gWeights[i + gBlurRadius];

			// Add neighbor pixel to blur.
			color += weight * volLightQuad.SampleLevel(
				samHM, tex, 0.0);

			totalWeight += weight;
		}
	}

	// Compensate for discarded samples by making total weights sum to 1.
	return color / totalWeight;
}
float4 bilateralBlurVer(psQuadInput pin) : SV_Target
{
	float2 texOffset = float2(0.0f, g_screenDimenstions.w);


	// The center value always contributes to the sum.
	float4 color = gWeights[5] * BilateralBlurHorQuad.SampleLevel(samHM, pin.uv, 0.0);
	float totalWeight = gWeights[5];

	float4 centerNormalDepth = gNormalDepthMap.SampleLevel(samHM, pin.uv, 0.0f);

	for (float i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		// We already added in the center weight.
		if (i == 0)
			continue;

		float2 tex = pin.uv + i * texOffset;

		float4 neighborNormalDepth = gNormalDepthMap.SampleLevel(
			samHM, tex, 0.0f);

		//
		// If the center value and neighbor values differ too much (either in 
		// normal or depth), then we assume we are sampling across a discontinuity.
		// We discard such samples from the blur.
		//

		if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f &&
			abs(neighborNormalDepth.a - centerNormalDepth.a) <= 0.2f)
		{
			float weight = gWeights[i + gBlurRadius];

			// Add neighbor pixel to blur.
			color += weight * BilateralBlurHorQuad.SampleLevel(
				samHM, tex, 0.0);

			totalWeight += weight;
		}
	}

	// Compensate for discarded samples by making total weights sum to 1.
	return color / totalWeight;
}