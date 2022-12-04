
#include "Common.hlsli"

Texture2D heightmap : register(t24);
SamplerState samHM : register(s4);
SamplerState sampleWrap : register(s0);
SamplerComparisonState sampleComparison : register(s1);
SamplerState sampleClamp : register(s2);
static const float g_minDistance = 1;
static const float g_maxDistance = 800;
static const float g_minTess = 0;
static const float g_maxTess  = 6;
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
HS_TERRAIN_CONSTANT_DATA_OUTPUT constantHS(InputPatch<PS_INPUT_TERRAIN, 4> input, uint i : SV_PrimitiveID)
{
	
	HS_TERRAIN_CONSTANT_DATA_OUTPUT patchTess;
	
	// We store the patch BoundsY in the first control point.
	float minY = input[0].bounds.x;
	float maxY = input[0].bounds.y;

	// Build axis-aligned bounding box.  patch[2] is lower-left corner
	// and patch[1] is upper-right corner.
	float3 vMin = float3(input[2].pos.x, minY, input[2].pos.z);
	float3 vMax = float3(input[1].pos.x, maxY, input[1].pos.z);

	float3 boxCenter = 0.5f * (vMin + vMax);
	float3 boxExtents = 0.5f * (vMax - vMin);
	if (AabbOutsideFrustumTest(boxCenter, boxExtents, g_frustumPlanes))
	{
		patchTess.edgeTessellation[0] = 0.0f;
		patchTess.edgeTessellation[1] = 0.0f;
		patchTess.edgeTessellation[2] = 0.0f;
		patchTess.edgeTessellation[3] = 0.0f;

		patchTess.insideTessellation[0] = 0.0f;
		patchTess.insideTessellation[1] = 0.0f;

		return patchTess;
	}


	float3 e0 = 0.5 * (input[0].pos + input[2].pos).xyz;
	float3 e1= 0.5 * (input[0].pos + input[1].pos).xyz;
	float3 e2= 0.5 * (input[1].pos + input[3].pos).xyz;
	float3 e3= 0.5 * (input[2].pos + input[3].pos).xyz;
	float3 c = 0.25 * (input[0].pos + input[1].pos + input[2].pos + input[3].pos).xyz;

	float d = distance(e0, g_cameraPosition.xyz);

	float tess = saturate((g_maxDistance - d) / (g_maxDistance - g_minDistance));

	patchTess.edgeTessellation[0] = g_minTess + (tess * (g_maxTess - g_minTess));

	d = distance(e1, g_cameraPosition.xyz);

	tess = saturate((g_maxDistance - d) / (g_maxDistance - g_minDistance));

	patchTess.edgeTessellation[1] = g_minTess + (tess * (g_maxTess - g_minTess));

	d = distance(e2, g_cameraPosition.xyz);

	tess = saturate((g_maxDistance - d) / (g_maxDistance - g_minDistance));

	patchTess.edgeTessellation[2] = g_minTess + (tess * (g_maxTess - g_minTess));

	d = distance(e3, g_cameraPosition.xyz);

	tess = saturate((g_maxDistance - d) / (g_maxDistance - g_minDistance));

	patchTess.edgeTessellation[3] = g_minTess + (tess * (g_maxTess - g_minTess));


	d = distance(c, g_cameraPosition.xyz);

	tess = saturate((g_maxDistance - d) / (g_maxDistance - g_minDistance));

	patchTess.insideTessellation[0] = g_minTess + (tess * (g_maxTess - g_minTess));
	
	patchTess.insideTessellation[1] = patchTess.insideTessellation[0];



	


	return patchTess;
}



float2 g_uvScale = 50;

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("constantHS")]
[maxtessfactor(64.0f)]
PS_INPUT_TERRAIN main(InputPatch<PS_INPUT_TERRAIN, 4> p, uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	PS_INPUT_TERRAIN output;
	output = p[i];

	return output;
}

