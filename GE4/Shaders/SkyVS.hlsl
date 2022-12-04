
#include "Common.hlsli"

INPUT_PS_SKY main(INPUT_VS input)
{
	INPUT_PS_SKY output;

	float4x4 mvp = mul(g_world, g_vp);

	output.pos = mul(float4(input.pos, 1), mvp).xyww;
	output.uv = input.pos;
	output.posW = mul(float4(input.pos, 1), g_world).xyz;
	output.index = 0;
	return output;
}