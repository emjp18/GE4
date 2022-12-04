
#include "Common.hlsli"
SamplerState samLinear : register(s0);
Texture2D tex  : register(t0);

float4 main(PARTICLE_INPUT_PS pin) : SV_TARGET
{
	
	float4 color = tex.Sample(samLinear, pin.Tex);
	color *= pin.Color;

	return color;
}

