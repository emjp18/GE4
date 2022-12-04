#include "Common.hlsli"

float3 pointFromLine(float along,float dist,float3 p1,float3 p2) {
	float3 res;
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	res.x = p1.x + dx * along - dy * dist;
	res.y = p1.y + dy * along + dx * dist;
	res.z = 0;
	return res;
}


Texture2D heightmap : register(t24);
Texture2D noiseMap : register (t15);
SamplerState sampleClamp : register(s2);
SamplerState samHM : register(s4);
[maxvertexcount(4)]
void main(line input_PS_STRAND input[2], inout TriangleStream<input_PS_STRAND> TriangleOutputStream)
{

	

	float3 tangent1 = normalize(input[1].tangent);
	float3 biNormal1 = normalize(input[1].biNormal);
	float3 normal1 = normalize(input[1].normal);

	float3 tangent = normalize(input[0].tangent);
	float3 biNormal = normalize(input[0].biNormal);
	float3 normal = normalize(input[0].normal);

	
	float3 look = normalize(g_cameraPosition.xyz - input[1].worldPos.xyz);
	
	float3 right = normalize(cross(float3(0, 1, 0), look));
	float3 up =  cross(look, right);

	float3 look1 = normalize(g_cameraPosition.xyz - (input[0].worldPos.xyz+input[0].displacement));

	float3 right1 =  normalize(cross(float3(0, 1, 0), look1));
	float3 up1 =  cross(look1, right1);
	
	const float PIXEL_SIZE = tan(0.5f * (PI / 2.0f)) / (float)g_screenDimenstions.y;

	float halfHeight = input[0].height*0.5;

	float halfHeight1 = input[1].height*0.5;
	float4 v[4];


	v[0] = float4(input[1].worldPos.xyz + (PIXEL_SIZE*1000) * right - halfHeight * up, 1.0f);
	v[1] = float4(input[0].worldPos.xyz + (PIXEL_SIZE * 100) * right1 + halfHeight1 * up1, 1.0f);
	v[2] = float4(input[1].worldPos.xyz - (PIXEL_SIZE * 1000) * right - halfHeight * up, 1.0f);
	v[3] = float4(input[0].worldPos.xyz - (PIXEL_SIZE * 100) * right1 + halfHeight1 * up1, 1.0f);



	normal = mul(float4(normal,0), g_world).xyz;
	normal1 = mul(float4(normal1,0), g_world).xyz;
	tangent = mul(float4(tangent, 0), g_world).xyz;
	tangent1 = mul(float4(tangent1, 0), g_world).xyz;
	biNormal = mul(float4(biNormal, 0), g_world).xyz;
	biNormal1 = mul(float4(biNormal1, 0), g_world).xyz;
	//normal = normal1 = float3(0, 1, 0);
	const float nFactor = 1;

	v[0].xyz += (nFactor * normalize(normal1));
	v[1].xyz += (nFactor * normalize(normal));
	v[2].xyz += (nFactor * normalize(normal1));
	v[3].xyz += (nFactor * normalize(normal));




	input_PS_STRAND output;
	

	//float4x4 mvp = mul(g_world, g_vp);
	float4x4 mvp = g_vp;


	//----------------------------------
	output.uv = input[0].uv;

	output.normal = normal;
	output.tangent = tangent;
	output.biNormal = biNormal;

	output.height = 1;
	output.displacement = 1;
	output.shadowPos = mul(v[0], g_lvpt);
	output.colour = float4(1, 1, 1, 1);
	output.vID = input[0].vID;

	//output.worldPos = mul(v[0], g_world).xyz;
	output.worldPos = v[0].xyz;
	output.position = mul(v[0], mvp);

	TriangleOutputStream.Append(output);

	//----------------------------------

	output.vID = input[0].vID;
	output.uv = input[1].uv;

	output.normal = normal1;
	output.tangent = tangent1;
	output.biNormal = biNormal1;

	output.shadowPos = mul(v[1], g_lvpt);
	output.height = 1;
	output.displacement = 1;



	//output.worldPos = mul(v[1], g_world).xyz;
	output.worldPos = v[1].xyz;
	output.position = mul(v[1], mvp);
	output.colour = float4(1, 1, 1, 1);
	TriangleOutputStream.Append(output);
	//----------------------------------


	//----------------------------------
	output.vID = input[0].vID;

	output.uv = input[0].uv;

	output.normal = normal;
	output.tangent = tangent;
	output.biNormal = biNormal;

	output.shadowPos = mul(v[2], g_lvpt);

	output.height = 1;
	output.displacement = 1;
	//output.worldPos = mul(v[2], g_world).xyz;
	output.worldPos = v[2].xyz;
	output.position = mul(v[2], mvp);
	output.colour = float4(1, 1, 1, 1);
	TriangleOutputStream.Append(output);
	//----------------------------------




	output.vID = input[0].vID;

	output.uv = input[1].uv;

	output.normal = normal1;
	output.tangent = tangent1;
	output.biNormal = biNormal1;

	output.shadowPos = mul(v[3], g_lvpt);
	output.height = 1;
	output.displacement = 1;


	//output.worldPos = mul(v[3], g_world).xyz;
	output.worldPos = v[3].xyz;
	output.position = mul(v[3], mvp);
	output.colour = float4(1, 1, 1, 1);

	TriangleOutputStream.Append(output);

	
	


}