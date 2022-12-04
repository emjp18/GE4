#include "Common.hlsli"
Texture2D heightmap : register(t24);
SamplerState samHM : register(s4);
input_PS mainInstance(INPUT_VS_INSTANCE input)
{

	input_PS output;
	float4x4 mvp = mul(input.inWorld, g_vp);
	output.pos = mul(float4(input.pos, 1), mvp);
	output.worldPos = mul(float4(input.pos, 1), input.inWorld).xyz;
	output.uv = input.uv;
	output.normal = mul(float4(input.normal, 0), transpose(input.inverseWorld)).xyz;
	output.tangent = mul(float4(input.tangent, 0), transpose(input.inverseWorld)).xyz;
	output.biNormal = mul(float4(input.biNormal, 0), transpose(input.inverseWorld)).xyz;
	output.shadowPos = mul(float4(output.worldPos, 1), g_lvpt);
	output.index = 0;
	output.posView = mul(float4(output.worldPos, 1), g_view).xyz;
	output.normalView = mul(float4(output.normal, 1), g_view).xyz;
	return output;

}

input_PS main(INPUT_VS input)
{

	input_PS output;
	float4x4 mvp = mul(g_world, g_vp);
	output.pos = mul(float4(input.pos, 1), mvp);
	output.worldPos = mul(float4(input.pos, 1), g_world).xyz;
	output.uv = input.uv;
	output.normal = mul(float4(input.normal, 0), transpose(g_inverseWorld)).xyz;
	output.tangent = mul(float4(input.tangent, 0), transpose(g_inverseWorld)).xyz;
	output.biNormal = mul(float4(input.biNormal, 0), transpose(g_inverseWorld)).xyz;
	output.shadowPos = mul(float4(output.worldPos, 1), g_lvpt);
	output.posView = mul(float4(output.worldPos, 1), g_view).xyz;
	output.normalView = mul(float4(output.normal, 1), g_view).xyz;
	output.index = 0;
	return output;

}


PS_INPUT_TERRAIN mainTerrain(INPUT_TERRAIN_VS input)
{

	PS_INPUT_TERRAIN output;
	output.pos = float4(input.pos,1); //already in ws
	output.pos.y = heightmap.SampleLevel(samHM, input.uv, 0).r;
	output.uv = input.uv;
	output.bounds = input.bounds;
	output.shadowPos = mul(float4(input.pos, 1), g_lvpt);
	output.worldPos = input.pos.xyz;
	output.tileUV = float2(0, 0);
	output.index = 0;
	output.posView = mul(float4(output.worldPos, 1), g_view).xyz;
	return output;

}

float4 mainEdgeInstance(INPUT_VS_INSTANCE input) : SV_POSITION
{


	float4x4 mvp = mul(input.inWorld, g_vp);
	float4 pos = mul(float4(input.pos, 1), mvp);
	float4 normal = mul(float4(input.normal, 0), mvp);
	return CEL(pos, normal);


}

float4 mainEdge(INPUT_VS input) : SV_POSITION
{


	float4x4 mvp = mul(g_world, g_vp);
	float4 pos = mul(float4(input.pos, 1), mvp);
	float4 normal = mul(float4(input.normal, 0), mvp);
	return CEL(pos, normal);


}