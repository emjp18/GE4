#include "Common.hlsli"

input_PS_STRAND main(INPUT_VS input)
{
	input_PS_STRAND output;


	float4x4 mvp = mul(g_world, g_vp);
	output.position = mul(float4(input.pos, 1), mvp);
	output.worldPos = mul(float4(input.pos, 1), g_world).xyz;
	output.uv = input.uv;
	output.normal =  mul(float4(input.normal, 0), transpose(g_inverseWorld)).xyz;
	output.tangent =  mul(float4(input.tangent, 0), transpose(g_inverseWorld)).xyz;
	output.biNormal =  mul(float4(input.biNormal, 0), transpose(g_inverseWorld)).xyz;
	output.shadowPos = mul(float4(output.worldPos, 1), g_lvpt);
	output.height = 1;
	output.displacement = 1;
	output.colour = float4(1, 1, 1, 1);
	output.vID = input.vID;
	return output;
}