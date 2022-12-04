

#include "Common.hlsli"

TextureCube cubeMap : register(t10);
SamplerState sampleWrap : register(s0);

float4 main(INPUT_PS_SKY input) : SV_TARGET
{

	return cubeMap.Sample(sampleWrap, input.uv);

	
}