
#include "Common.hlsli"

SamplerState samLinear : register(s0);
Texture1D randomTex : register(t17);

[maxvertexcount(4)]
void fire2(point PARTICLE_INPUT_GS gin[1],
	inout TriangleStream<PARTICLE_INPUT_PS> triStream)
{

	if (gin[0].Type != PT_EMITTER) {

		

		// Compute world matrix so that billboard faces the camera		
		float3 look = normalize(g_cameraPosition.xyz - gin[0].PosW);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		// Compute triangle strip vertices (quad) in world space
		float halfWidth = 0.5f * g_particleConstants.x;
		float halfHeight = 0.5f * g_particleConstants.y;

		

		float4 v[4];
		v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * up, 1.0f);
		v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * up, 1.0f);
		v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * up, 1.0f);
		v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * up, 1.0f);

		PARTICLE_INPUT_PS gOut[4];
		
		gOut[0].PosH = mul(v[0], g_vp);
		gOut[1].PosH = mul(v[1], g_vp);
		gOut[2].PosH = mul(v[2], g_vp);
		gOut[3].PosH = mul(v[3], g_vp);

		gOut[0].Type = gin[0].Type;
		gOut[1].Type = gin[0].Type;
		gOut[2].Type = gin[0].Type;
		gOut[3].Type = gin[0].Type;



		gOut[0].Tex = float2(0.0f, 1.0f);
		gOut[1].Tex = float2(1.0f, 1.0f);
		gOut[2].Tex = float2(0.0f, 0.0f);
		gOut[3].Tex = float2(1.0f, 0.0f);

		gOut[0].Color = gin[0].Color;
		gOut[1].Color = gin[0].Color;
		gOut[2].Color = gin[0].Color;
		gOut[3].Color = gin[0].Color;

		gOut[0].Age = gin[0].Age;
		gOut[1].Age = gin[0].Age;
		gOut[2].Age = gin[0].Age;
		gOut[3].Age = gin[0].Age;

		gOut[0].PosW = v[0].xyz;
		gOut[1].PosW = v[1].xyz;
		gOut[2].PosW = v[2].xyz;
		gOut[3].PosW = v[3].xyz;
		for (int i = 0; i < 4; ++i)
		{
			triStream.Append(gOut[i]);
		}
	}
}

float3 RandUnitVec3(float offset)
{

	float u = (g_time.x + offset);
	float3 v = randomTex.SampleLevel(samLinear, u, 0).xyz;


	return normalize(v);
}

float3 RandVec3(float offset)
{

	float u = (g_time.x + offset);
	float3 v = randomTex.SampleLevel(samLinear, u, 0).xyz;


	return v;
}

[maxvertexcount(4)]
void fire(point Particle gin[1], inout PointStream<Particle> ptStream)
{
	gin[0].Age += g_time.y;


	if (gin[0].Type == PT_EMITTER)
	{

		if (gin[0].Age > 0.05f)
		{
			const float particleSize = 30.0f;;
			const float particleSpreadF = 0.5f;
			float3 vRandom = RandUnitVec3(0.0f);
			vRandom.x *= particleSpreadF;
			vRandom.z *= particleSpreadF;
			Particle p;
			p.InitialPosW = g_particleWP.xyz;
			p.InitialVelW = 4.0* vRandom;
			p.SizeW.x = g_particleConstants.x;
			p.SizeW.y = g_particleConstants.y;
			p.Age = gin[0].Age;
			p.Type = PT_FLARE;

			ptStream.Append(p);

			gin[0].Age = 0.0f;
		
		}
		

		ptStream.Append(gin[0]);

	}
	else
	{
		const float particleMaxAge = 1.0f;
		if (gin[0].Age <= particleMaxAge)
			ptStream.Append(gin[0]);
	}

}

[maxvertexcount(6)]
void leaf(point Particle gin[1], inout PointStream<Particle> ptStream) // need another one of these for leaves to not follow the camera but another point like a tree
{
	gin[0].Age += (g_time.y)*3;


	if (gin[0].Type == PT_EMITTER)
	{


		if (gin[0].Age > 0.05)
		{

			float3 vRandom = 35.0f * RandVec3((float)1 / 1.0f);
			vRandom.y = 20.0f;

			Particle p;
			p.InitialPosW = g_particleWP.xyz + vRandom;
			p.InitialPosW.z += vRandom.z;
			p.InitialVelW = float3(0.0f, 0.0f, 0.0f);
			p.SizeW = float2(g_particleConstants.x, g_particleConstants.y);
			p.Age = 0.0f;
			p.Type = PT_FLARE;
			ptStream.Append(p);

			gin[0].Age = 0.0f;
		}


		ptStream.Append(gin[0]);

	}
	else
	{
		const float particleMaxAge = 5.0f;
		if (gin[0].Age <= particleMaxAge)
			ptStream.Append(gin[0]);
	}

}



[maxvertexcount(6)]
void rain(point Particle gin[1], inout PointStream<Particle> ptStream) 
{
	gin[0].Age += (g_time.y)*10;


	if (gin[0].Type == PT_EMITTER)
	{


		if (gin[0].Age > 0.05)
		{

			for (int i = 0; i < 2; i++)
			{
				float3 vRandom = 35.0f * RandVec3((float)i / 1.0f);
				vRandom.y = 20.0f;

				Particle p;
				p.InitialPosW = g_cameraPosition.xyz + vRandom;
				p.InitialPosW.z += 0;
				p.InitialVelW = float3(0.0f, 0.0f, 0.0f);
				p.SizeW = float2(g_particleConstants.x, g_particleConstants.y);
				p.Age = 0.0f;
				p.Type = PT_FLARE;
				ptStream.Append(p);

			}

			gin[0].Age = 0.0f;
		}


		ptStream.Append(gin[0]);

	}
	else
	{
		const float particleMaxAge = 5.0f;
		if (gin[0].Age <= particleMaxAge)
			ptStream.Append(gin[0]);
	}

}

[maxvertexcount(2)]
void rain2(point PARTICLE_INPUT_PS gin[1],
	inout LineStream<PARTICLE_INPUT_PS> lineStream)
{

	if (gin[0].Type != PT_EMITTER)
	{
		
		float3 p0 = gin[0].PosW;
		float3 p1 = gin[0].PosW + 0.07f * gAccelWR;
		PARTICLE_INPUT_PS v0;
		v0.PosH = mul(float4(p0, 1.0f), g_vp);
		v0.Tex = float2(0.0f, 0.0f);
		v0.Color = float4(1, 1, 1, 1);
		v0.Age = 1;
		v0.PosW = p0;
		v0.Type = gin[0].Type;
		lineStream.Append(v0);

		PARTICLE_INPUT_PS v1;
		v1.PosH = mul(float4(p1, 1.0f), g_vp);
		v1.Tex = float2(1.0f, 1.0f);
		v1.Color = float4(1, 1, 1, 1);
		v1.Age = 1;
		v1.PosW = p0;
		v1.Type = gin[0].Type;
		lineStream.Append(v1);
		
	}
}

[maxvertexcount(4)]
void textBubble(point Particle gin[1],
	inout TriangleStream<PARTICLE_INPUT_PS> triStream
)
{
	float3 pos = g_particleWP;
	// Compute world matrix so that billboard faces the camera		
	float3 look = normalize(g_cameraPosition.xyz - pos);
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up = cross(look, right);

	// Compute triangle strip vertices (quad) in world space
	float halfWidth = 0.5f * g_particleConstants.x;
	float halfHeight = 0.5f * g_particleConstants.y;



	float4 v[4];
	v[0] = float4(pos + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(pos + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(pos - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(pos - halfWidth * right + halfHeight * up, 1.0f);

	PARTICLE_INPUT_PS gOut[4];
	/*float4x4 mvp = mul(g_world, g_vp);*/
	gOut[0].PosH = mul(v[0], g_vp);
	gOut[1].PosH = mul(v[1], g_vp);
	gOut[2].PosH = mul(v[2], g_vp);
	gOut[3].PosH = mul(v[3], g_vp);

	gOut[0].Tex = float2(0.0f, 1.0f);
	gOut[1].Tex = float2(1.0f, 1.0f);
	gOut[2].Tex = float2(0.0f, 0.0f);
	gOut[3].Tex = float2(1.0f, 0.0f);

	gOut[0].Type = 1;
	gOut[1].Type = 1;
	gOut[2].Type = 1;
	gOut[3].Type = 1;

	gOut[0].Color = 1;
	gOut[1].Color = 1;
	gOut[2].Color = 1;
	gOut[3].Color = 1;

	gOut[0].Age = 1;
	gOut[1].Age = 1;
	gOut[2].Age = 1;
	gOut[3].Age = 1;

	gOut[0].PosW = v[0].xyz;
	gOut[1].PosW = v[1].xyz;
	gOut[2].PosW = v[2].xyz;
	gOut[3].PosW = v[3].xyz;

	for (int i = 0; i < 4; ++i)
	{
		triStream.Append(gOut[i]);
	}
}

[maxvertexcount(4)]
void leaf2(point PARTICLE_INPUT_PS gin[1],
	inout TriangleStream<PARTICLE_INPUT_PS> triStream
)
{
	if (gin[0].Type != PT_EMITTER)
	{
		
		float3 pos = gin[0].PosW;
		// Compute world matrix so that billboard faces the camera		
		float3 look = normalize(g_cameraPosition.xyz - pos);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		// Compute triangle strip vertices (quad) in world space
		float halfWidth = 0.5f * g_particleConstants.x;
		float halfHeight = 0.5f * g_particleConstants.y;



		float4 v[4];
		v[0] = float4(pos + halfWidth * right - halfHeight * up, 1.0f);
		v[1] = float4(pos + halfWidth * right + halfHeight * up, 1.0f);
		v[2] = float4(pos - halfWidth * right - halfHeight * up, 1.0f);
		v[3] = float4(pos - halfWidth * right + halfHeight * up, 1.0f);

		PARTICLE_INPUT_PS gOut[4];
		/*float4x4 mvp = mul(g_world, g_vp);*/
		gOut[0].PosH = mul(v[0], g_vp);
		gOut[1].PosH = mul(v[1], g_vp);
		gOut[2].PosH = mul(v[2], g_vp);
		gOut[3].PosH = mul(v[3], g_vp);

		gOut[0].Tex = float2(0.0f, 1.0f);
		gOut[1].Tex = float2(1.0f, 1.0f);
		gOut[2].Tex = float2(0.0f, 0.0f);
		gOut[3].Tex = float2(1.0f, 0.0f);

		gOut[0].Type = 1;
		gOut[1].Type = 1;
		gOut[2].Type = 1;
		gOut[3].Type = 1;

		gOut[0].Color = 1;
		gOut[1].Color = 1;
		gOut[2].Color = 1;
		gOut[3].Color = 1;

		gOut[0].Age = 1;
		gOut[1].Age = 1;
		gOut[2].Age = 1;
		gOut[3].Age = 1;

		gOut[0].PosW = v[0].xyz;
		gOut[1].PosW = v[1].xyz;
		gOut[2].PosW = v[2].xyz;
		gOut[3].PosW = v[3].xyz;

		for (int i = 0; i < 4; ++i)
		{
			triStream.Append(gOut[i]);
		}
	}
	
}