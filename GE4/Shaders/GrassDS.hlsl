#include "Common.hlsli"
float g_texelCellU = 1.0f / (float)HM_W;
float g_texelCellV = 1.0f / (float)HM_H;
float g_worldCell = (float)CELL_SPACING;

Texture2D heightmap : register(t24);
Texture2D noiseMap : register (t15);
SamplerState sampleClamp : register(s2);
SamplerState samHM : register(s4);

float3 bezier( float3 p0, float3 p1, float3 p2, float3 p3, float u)
{
	float B0 = (1. - u) * (1. - u) * (1. - u);
	float B1 = 3. * u * (1. - u) * (1. - u);
	float B2 = 3. * u * u * (1. - u);
	float B3 = u * u * u;

	float3 p = B0 * p0 + B1 * p1 + B2 * p2 + B3 * p3;
	return p;

}

float3 bezier2(float3 a, float3 b, float t) {
	return lerp(a, b, t);
}
float3 bezier3(float3 a, float3 b, float3 c, float t) {
	return lerp(bezier2(a, b, t), bezier2(b, c, t), t);
}
float3 bezier4(float3 a, float3 b, float3 c, float3 d, float t) {
	return lerp(bezier3(a, b, c, t), bezier3(b, c, d, t), t);
}

[domain("isoline")]
input_PS_STRAND main(HS_STRAND_CONSTANT_DATA_OUTPUT input,
	OutputPatch<input_PS_STRAND, 4> inp,
	float2 uv : SV_DomainLocation,  uint i : SV_PrimitiveID)
{
	input_PS_STRAND output;
	

	
	

	

	output.uv = lerp(lerp(inp[0].uv, inp[1].uv, uv.x), lerp(inp[2].uv, inp[3].uv, uv.x), uv.y);

	output.normal = lerp(lerp(inp[0].normal, inp[1].normal, uv.x), lerp(inp[2].normal, inp[3].normal, uv.x), uv.y);

	output.tangent = lerp(lerp(inp[0].tangent, inp[1].tangent, uv.x), lerp(inp[2].tangent, inp[3].tangent, uv.x), uv.y);

	output.biNormal = lerp(lerp(inp[0].biNormal, inp[1].biNormal, uv.x), lerp(inp[2].biNormal, inp[3].biNormal, uv.x), uv.y);

	output.shadowPos = lerp(lerp(inp[0].shadowPos, inp[1].shadowPos, uv.x), lerp(inp[2].shadowPos, inp[3].shadowPos, uv.x), uv.y);

	float3 pos = lerp(lerp(inp[0].worldPos, inp[1].worldPos, uv.x), lerp(inp[2].worldPos, inp[3].worldPos, uv.x), uv.y);

	//float3 pos1 = pow(1.0f - t, 3.0f)* inp[0].worldPos + 3.0f * pow(1.0f - t, 2.0f) * t * inp[1].worldPos + 3.0f * (1.0f - t) * pow(t, 2.0f) * inp[2].worldPos + pow(t, 3.0f) * inp[3].worldPos;
	float t = uv.x;
	//float3 pos = bezier(inp[0].worldPos, inp[1].worldPos, inp[2].worldPos, inp[3].worldPos, t);

	float randomValue = noiseMap.SampleLevel(sampleClamp, output.uv, 0).r;

	output.height = g_grassConstants.x * (randomValue);
	output.displacement = randomValue * sin(g_time.x);
	output.position = float4(pos, 1);
	
	output.worldPos = pos;
	output.colour = float4(1, 1, 1, 1);
	output.vID = inp[0].vID;
	return output;
}




