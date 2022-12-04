
#include "Common.hlsli"

PARTICLE_INPUT_GS fire(Particle vin)
{
	PARTICLE_INPUT_GS vout;

	float t = vin.Age;
	

	vout.PosW = 0.5f * t * t * gAccelW + t * vin.InitialVelW + vin.InitialPosW;

	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / 1.0f);
	vout.Color = float4(1, 1, 1, opacity);

	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;
	vout.Age = vin.Age;

	return vout;
}

Particle main(Particle vin)
{

	return vin;
}

PARTICLE_INPUT_PS rain(Particle vin)
{
	PARTICLE_INPUT_PS vout;
	float t = vin.Age;
	// constant acceleration equation 
	vout.PosW = 0.5f * t * t * gAccelWR + t * vin.InitialVelW + vin.InitialPosW;
	vout.Type = vin.Type;
	vout.Color = float4(1, 1, 1, 1);
	vout.Age = 1;
	vout.PosH = float4(1, 1, 1, 1);
	return vout;
}
