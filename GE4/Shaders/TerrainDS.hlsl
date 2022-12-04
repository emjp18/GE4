#include "Common.hlsli"


Texture2D heightmap : register(t24);
SamplerState samHM : register(s4);


float2 g_uvScale = 50;


[domain("quad")]
PS_INPUT_TERRAIN main(HS_TERRAIN_CONSTANT_DATA_OUTPUT patchTess,
	float2 uv : SV_DomainLocation,
	const OutputPatch<PS_INPUT_TERRAIN, 4> quad)
{
	PS_INPUT_TERRAIN output;

	output.bounds = quad[0].bounds;
	output.worldPos = lerp(lerp(quad[0].pos.xyz, quad[1].pos.xyz, uv.x), lerp(quad[2].pos.xyz, quad[3].pos.xyz, uv.x), uv.y);
	output.uv = lerp(lerp(quad[0].uv, quad[1].uv, uv.x), lerp(quad[2].uv, quad[3].uv, uv.x), uv.y);
	output.tileUV = output.uv * g_uvScale;
	output.worldPos.y = heightmap.SampleLevel(samHM, output.uv, 0).r;
	output.pos = mul(float4(output.worldPos, 1), g_vp);
	output.shadowPos = lerp(lerp(quad[0].shadowPos, quad[1].shadowPos, uv.x), lerp(quad[2].shadowPos, quad[3].shadowPos, uv.x), uv.y);
	output.index = quad[0].index;
	output.posView = lerp(lerp(quad[0].posView.xyz, quad[1].posView.xyz, uv.x), lerp(quad[2].posView.xyz, quad[3].posView.xyz, uv.x), uv.y);
	return output;
}


