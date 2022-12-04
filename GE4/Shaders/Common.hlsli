#include "CommonMath.hlsli"


#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
#define BLOCK_SIZE 16
#define HM_W 2048
#define HM_H 2048
#define PLAYER_INSTANCE 1
#define GRASS_PER_TRIANGLE 64
#define PT_EMITTER 0
#define PT_FLARE 1
#define SHADOW 1
#define NO_SHADOW 0
#define MAX_LIGHTS 20
#define NB_STEPS 10
#define INTENSITY_BOOST 10
#define EDGE_THRESHOLD_MIN 0.0312f
#define EDGE_THRESHOLD_MAX 0.125f
#define ITERATIONS 12
#define SUBPIXEL_QUALITY 0.75f
#define CELL_SPACING 0.5

cbuffer CB_OBJECT : register(b0)
{
	row_major float4x4 g_world;
	row_major float4x4 g_inverseWorld; //this should not b transposed so that it becomes transposed.
	float4 g_particleWP; 
};

cbuffer CB_SCENE: register(b1)
{
	row_major float4x4 g_view;
	row_major float4x4 g_projection;
	row_major float4x4 g_inverseView;
	row_major float4x4 g_inverseProjection;
	row_major float4x4 g_vp;
	row_major float4x4 g_lvpt;
	float4 g_time;//y is timestep
	uint4 g_lightCount;//x is only used
	float4 g_dirLightColor;
	float4 g_dirLightOtherConstants;//x is intensity and w is if it has shadow
	float4 g_dirLightDirection;
	float4 g_cameraPosition;
	uint4 g_screenDimenstions;//x and y are window dimensions, z and w are texel size.
	float4 g_frustumPlanes[6];
	row_major float4x4 g_shadowVP;
	float4 g_booleans; //x is cel shading, 1 is true, 0 is false
};


cbuffer CB_GRASS : register(b3)
{
	float4 g_grassConstants; //x height, y width,z heightmap
};

cbuffer CB_MORPH: register(b4)
{
	float4 g_mWeights;
};

cbuffer CB_SHADOW_POINT : register(b5)
{
	row_major float4x4 g_vpPL[6];
};


cbuffer CB_WATER : register(b6)
{
	row_major float4x4 g_texTransform[5];
};

cbuffer CB_PARTICLE : register(b7)
{
	float4 g_particleConstants; //x,y, width height,
};

struct Light
{
	float4 position;
	float4 color;
	uint4 type;//x is the type,
	float4 direction;
	float4 otherConstants;//x is intensity, y is range z is fov w is if it has shadow
};

struct MORPH_VERTEX
{
	float3 pos;
	float3 normal;
	float3 tangent;
	float3 biNormal;
};


struct GrassStrand
{
	float3 pos;
	float2 uv;
	float3 normal;
	float3 tangent;
	float3 biNormal;
};

StructuredBuffer<Light> SB_LIGHT : register(t7);
StructuredBuffer<float4x4> SB_JOINT: register(t28); //must transpose these.
StructuredBuffer<GrassStrand> SB_GRASS_STRAND : register(t27);
StructuredBuffer<uint> SB_LIGHT_INDEX : register(t29);
StructuredBuffer<uint> SB_GRASS_INDEX : register(t32);
StructuredBuffer<float3> SB_GRASS_BC : register(t31);
StructuredBuffer<MORPH_VERTEX> SB_MORPH_ONE : register(t12);
StructuredBuffer<MORPH_VERTEX> SB_MORPH_TWO : register(t13);
StructuredBuffer<MORPH_VERTEX> SB_MORPH_THREE : register(t14);
StructuredBuffer<uint3> SB_MORPH_INDEX : register(t21);


struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age : AGE;
	uint Type          : TYPE;
};

struct PARTICLE_INPUT_GS
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
	float Age : AGE;
};

struct INPUT_VS
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	uint4 jointNr : JOINTNR;
	float4 skinWeight : SKINWEIGHT;
	uint vID : SV_VertexID;

};

struct INPUT_TERRAIN_VS
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float2 bounds : BOUNDS;
	uint vID : SV_VertexID;
	
};
struct HS_TERRAIN_CONSTANT_DATA_OUTPUT
{
	float edgeTessellation[4]: SV_TessFactor;
	float insideTessellation[2] : SV_InsideTessFactor;
};

struct INPUT_VS_INSTANCE
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	uint4 jointNr : JOINTNR;
	float4 skinWeight : SKINWEIGHT;
	row_major float4x4 inWorld : INSTANCEWORLD;
	float4x4 inverseWorld : INSTANCEINVERSEWORLD;
	uint vID : SV_VertexID;
	uint InstanceID : SV_InstanceID;
};

struct input_PS
{
	float4 pos : SV_POSITION;
	float3 worldPos : WORLDPOS;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	float4 shadowPos : TEXCOORD1;
	float3 posView : POSVIEW;
	float3 normalView : NORMALVIEW;
	uint index : SV_RenderTargetArrayIndex;

};

struct psQuadInput
{
	float2 uv : TEXCOORD;
	float4 pos : SV_POSITION;
};

struct Input_Blur_PS
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float2 texCoord[11] : TEXCOORD1;
};

struct HS_INPUT_STRAND
{
	float tessFactor : TESSFACTOR;
	float3 pos : WORLDPOS;
	float2 uv : TEXCOORD;
	float2 bounds : TEXCOORD1;
	uint vID : INDEX;
};
struct HS_STRAND_CONSTANT_DATA_OUTPUT
{
	float edgeTesselation[2]: SV_TessFactor;
	//uint triangleIndex : TRI_INDEX;
};
//
struct DS_INPUT_STRAND
{
	uint index : TRI_INDEX;
	float3 pos : WORLDPOS;
	float2 uv : TEXCOORD;
	uint vID : INDEX;
};
struct PS_INPUT_TERRAIN
{
	float4 pos :SV_POSITION;
	float3 worldPos : WORLDPOS;
	float2 uv : TEXCOORD;
	float2 tileUV : TEXCOORD1;
	float4 shadowPos : TEXCOORD2;
	float2 bounds : BOUNDS;
	uint index : SV_RenderTargetArrayIndex;
	float3 posView : POSVIEW;
	
};
struct GS_INPUT_STRAND
{
	float4 position	: SV_POSITION;
	centroid float2 uv	: TEXCOORD;
	float  height : GRASS_HEIGHT;
	float displacement : DISPLACMENT;
	/*float u : LERPVALUE0;
	float v : LERPVALUE1;*/
};

struct input_PS_STRAND
{
	float3 worldPos : WORLDPOS;
	float4 colour : COLOUR;
	float4 position : SV_POSITION;
	float2 uv : TEX_COORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	float4 shadowPos : TEXCOORD1;
	float  height : GRASS_HEIGHT;
	float displacement : DISPLACMENT;
	uint vID : VERINDEX;

};



struct PARTICLE_INPUT_PS
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	float2 Tex   : TEXCOORD;
	float Age : AGE;
	uint Type          : TYPE;
	float3 PosW  : POSITION;
};





struct INPUT_PS_SKY
{
	float4 pos : SV_POSITION;
	float3 uv : TEXCOORD;
	float3 posW : WORLDPOS;
	uint index : SV_RenderTargetArrayIndex;
};

struct INPUT_HS_WATER
{
	float2 uv : TEXCOORD;
	float2 uvN1 : TEXCOORD1;
	float2 uvN2 : TEXCOORD2;
	float2 uvD1 : TEXCOORD3;
	float2 uvD2 : TEXCOORD4;
	float tessFactor : TESSFACTOR;
	float3 worldPos : WORLDPOS;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;

};

struct INPUT_PS_WATER
{
	float2 uv : TEXCOORD;
	float2 uvN1 : TEXCOORD1;
	float2 uvN2 : TEXCOORD2;
	float2 uvD1 : TEXCOORD3;
	float2 uvD2 : TEXCOORD4;
	float4 pos : SV_POSITION;
	float3 worldPos : WORLDPOS;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
};


struct INPUT_PS_RAYMARCH
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 viewDir : TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 color: SV_Target0;
	float4 emissive  : SV_Target1;
	float4 normalView : SV_Target2;
	float4 worldPos : SV_Target3;
};
struct PS_QUAD_OUTPUT
{
	float4 color: SV_RENDERTARGET0;
	float4 vol : SV_RENDERTARGET1;
};


