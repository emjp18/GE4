

#include "Common.hlsli"

input_PS main(INPUT_VS input)
{
	input_PS output;

	float w1 = saturate(1 - (input.skinWeight.y + input.skinWeight.z));
	float w2 = saturate(1 - (w1 + input.skinWeight.z));
	float w3 = saturate(1 - (w2 + w1));

	float4 p = mul(float4(input.pos, 1), SB_JOINT[input.jointNr.x]) * w1;
	p += mul(float4(input.pos, 1), SB_JOINT[input.jointNr.y]) * w2;
	p += mul(float4(input.pos, 1), SB_JOINT[input.jointNr.z]) * w3;

	float3 n = mul(float4(input.normal, 0), SB_JOINT[input.jointNr.x]).xyz * w1;
	n += mul(float4(input.normal, 0), SB_JOINT[input.jointNr.y]).xyz * w2;
	n += mul(float4(input.normal, 0), SB_JOINT[input.jointNr.z]).xyz * w3;

	float3 t = mul(float4(input.tangent, 0), SB_JOINT[input.jointNr.x]).xyz * w1;
	t += mul(float4(input.tangent, 0), SB_JOINT[input.jointNr.y]).xyz * w2;
	t += mul(float4(input.tangent, 0), SB_JOINT[input.jointNr.z]).xyz * w3;

	float3 b = mul(float4(input.biNormal, 0), SB_JOINT[input.jointNr.x]).xyz * w1;
	b += mul(float4(input.biNormal, 0), SB_JOINT[input.jointNr.y]).xyz * w2;
	b += mul(float4(input.biNormal, 0), SB_JOINT[input.jointNr.z]).xyz * w3;

	float4x4 mvp = mul(g_world, g_vp);
	float4 outpos = mul(float4(p.xyz, 1), mvp);
	float3 outputWorldpos = mul(p, g_world).xyz;

	output.uv = input.uv;
	output.worldPos = outputWorldpos;
	output.pos = outpos;
	output.biNormal = normalize(mul(float4(b, 0), g_world).xyz);
	output.normal = normalize(mul(float4(n, 0), g_world).xyz);
	output.tangent = normalize(mul(float4(t, 0), g_world).xyz);
	output.shadowPos = mul(p, g_lvpt);
	output.index = 0;
	output.posView = mul(float4(output.worldPos, 1), g_view).xyz;
	output.normalView = mul(float4(output.normal, 1), g_view).xyz;
	output.index = 0;

	return output;
}


float4 mainEdgeSkeleton(INPUT_VS input) : SV_POSITION
{
	float w1 = saturate(1 - (input.skinWeight.y + input.skinWeight.z));
	float w2 = saturate(1 - (w1 + input.skinWeight.z));
	float w3 = saturate(1 - (w2 + w1));

	float4 p = mul(float4(input.pos, 1), SB_JOINT[input.jointNr.x]) * w1;
	p += mul(float4(input.pos, 1), SB_JOINT[input.jointNr.y]) * w2;
	p += mul(float4(input.pos, 1), SB_JOINT[input.jointNr.z]) * w3;

	float3 n = mul(float4(input.normal, 0), SB_JOINT[input.jointNr.x]).xyz * w1;
	n += mul(float4(input.normal, 0), SB_JOINT[input.jointNr.y]).xyz * w2;
	n += mul(float4(input.normal, 0), SB_JOINT[input.jointNr.z]).xyz * w3;



	float4x4 mvp = mul(g_world, g_vp);
	float4 pos = mul(float4(p.xyz, 1), mvp);



	float4 normal = normalize(mul(float4(n, 0), mvp));
	return CEL(pos, normal);

}