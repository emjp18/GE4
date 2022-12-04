
#include "Common.hlsli"

Texture2D gBlendMap: register(t25);
Texture2D heightmap : register(t24);
SamplerState samHM : register(s4);
SamplerState sampleWrap : register(s0);
SamplerComparisonState sampleComparison : register(s1);
SamplerState sampleClamp : register(s2);

static const float g_minDistance = 1;
static const float g_maxDistance = 600;
static const float g_minTess = 0;
static const float g_maxTess = 32;
float calcTessFactor(float3 p)
{
	float d = distance(p, g_cameraPosition.xyz);
	float s = saturate((d - g_minDistance) / (g_maxDistance - g_minDistance));
	return pow(2, (lerp(g_maxTess, g_minTess, s)));
}
bool AabbBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
	float3 n = abs(plane.xyz);

	// This is always positive.
	float r = dot(extents, n);

	// signed distance from center point to plane.
	float s = dot(float4(center, 1.0f), plane);

	// If the center point of the box is a distance of e or more behind the
	// plane (in which case s is negative since it is behind the plane),
	// then the box is completely in the negative half space of the plane.
	return (s + r) < 0.0f;
}
bool AabbOutsideFrustumTest(float3 center, float3 extents, float4 frustumPlanes[6])
{
	for (int i = 0; i < 6; ++i)
	{
		// If the box is completely behind any of the frustum planes
		// then it is outside the frustum.
		if (AabbBehindPlaneTest(center, extents, frustumPlanes[i]))
		{
			return true;
		}
	}

	return false;
}
HS_STRAND_CONSTANT_DATA_OUTPUT hsPerIsoLinePatch(InputPatch<input_PS_STRAND, 4> input, uint i : SV_PrimitiveID)
{
	HS_STRAND_CONSTANT_DATA_OUTPUT output;


	

	float3 c = (input[0].worldPos + input[1].worldPos + input[2].worldPos+ input[3].worldPos) * 0.25f;


	output.edgeTesselation[1] =  1;

	 

	float d = distance(c, g_cameraPosition.xyz);

	float tess = saturate((g_maxDistance - d) / (g_maxDistance - g_minDistance));

	output.edgeTesselation[0] = g_minTess + (tess * (g_maxTess - g_minTess));


	return output;
}

[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(4)]
[patchconstantfunc("hsPerIsoLinePatch")]
input_PS_STRAND main(InputPatch<input_PS_STRAND, 4> p,
	uint i : SV_OutputControlPointID)
{
	input_PS_STRAND output;
	output = p[i];

	return output;
}
