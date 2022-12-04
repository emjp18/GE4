#include "Common.hlsli"

#define FLT_MAX         3.402823466e+38F


#define NUM_THREADS_X BLOCK_SIZE
#define NUM_THREADS_Y BLOCK_SIZE
#define NUM_THREADS_PER_TILE (NUM_THREADS_X*NUM_THREADS_Y)


SamplerState sampleClamp : register(s2);
Texture2D LightCountHeatMap : register(t6);
Texture2D depthTexture : register(t0);

globallycoherent RWStructuredBuffer<uint> LightIndexCounter : register(u0);
RWStructuredBuffer<uint> lightIndexList : register(u1);
RWTexture2D<uint2> lightGrid : register(u2);
RWTexture2D<float4> DebugTexture : register(u3);


groupshared uint ldsZMax;
groupshared uint ldsZMin;
groupshared uint LightCount;
groupshared uint LightIndexStartOffset;
groupshared uint LightList[1024];

float3 CreatePlaneEquation(float3 b, float3 c)
{
    // normalize(cross( b-a, c-a )), except we know "a" is the origin
    // also, typically there would be a fourth term of the plane equation, 
    // -(n dot a), except we know "a" is the origin
    return normalize(cross(b, c));;
}

// point-plane distance, simplified for the case where 
// the plane passes through the origin
float GetSignedDistanceFromPlane(float3 p, float3 eqn)
{
    // dot(eqn.xyz,p) + eqn.w, , except we know eqn.w is zero 
    // (see CreatePlaneEquation above)
    return dot(eqn, p);
}

bool TestFrustumSides(float3 c, float r, float3 plane0, float3 plane1, float3 plane2, float3 plane3)
{
    bool intersectingOrInside0 = GetSignedDistanceFromPlane(c, plane0) < r;
    bool intersectingOrInside1 = GetSignedDistanceFromPlane(c, plane1) < r;
    bool intersectingOrInside2 = GetSignedDistanceFromPlane(c, plane2) < r;
    bool intersectingOrInside3 = GetSignedDistanceFromPlane(c, plane3) < r;

    return (intersectingOrInside0 && intersectingOrInside1 &&
        intersectingOrInside2 && intersectingOrInside3);
}

// calculate the number of tiles in the horizontal direction
uint GetNumTilesX()
{
    return (uint)((g_screenDimenstions.x + BLOCK_SIZE - 1) / (float)BLOCK_SIZE);
}

// calculate the number of tiles in the vertical direction
uint GetNumTilesY()
{
    return (uint)((g_screenDimenstions.y + BLOCK_SIZE - 1) / (float)BLOCK_SIZE);
}

// convert a point from post-projection space into view space
float4 ConvertProjToView(float4 p)
{
    p = mul(p, g_inverseProjection);
    p /= p.w;
    return p;
}

// convert a depth value from post-projection space into view space
float ConvertProjDepthToView(float z)
{
    z = 1.f / (z * g_inverseProjection._34 + g_inverseProjection._44);
    return z;
}

void CalculateMinMaxDepthInLds(uint3 globalThreadIdx)
{
    float depth = depthTexture.Load(uint3(globalThreadIdx.x, globalThreadIdx.y, 0)).x;
    float viewPosZ = ConvertProjDepthToView(depth);
    uint z = asuint(viewPosZ);
    if (depth != 0.f)
    {
        InterlockedMax(ldsZMax, z);
        InterlockedMin(ldsZMin, z);
    }
}

struct ComputeShaderInput
{
	uint3 groupID           : SV_GroupID;
	uint3 groupThreadID     : SV_GroupThreadID;
	uint3 dispatchThreadID  : SV_DispatchThreadID;
	uint  groupIndex        : SV_GroupIndex;
};
float4 ClipToView(float4 clip) //I have them here so I don't have to pass in the constants as parameters.
{

	float4 view = mul(g_inverseProjection, clip);

	view = view / view.w;

	return view;
}


float4 ScreenToView(float4 screen)
{

	float2 texCoord = screen.xy / g_screenDimenstions.xy;

    //	float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
	float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

	return ClipToView(clip);
}






// Add the light to the visible light list for opaque geometry.
void AppendLight(uint lightIndex)
{
    uint index; // Index into the visible lights array.
    InterlockedAdd(LightCount, 1, index); //new value, increament, old value
    if (index < 1024)
    {
     
        LightList[index] = lightIndex;
    }
}






[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{
    int2 texCoord = IN.dispatchThreadID.xy;
    if (IN.dispatchThreadID.x == 0 && IN.dispatchThreadID.y == 0 && IN.dispatchThreadID.z == 0)
    {
        LightIndexCounter[0] = 0;
    }
    GroupMemoryBarrierWithGroupSync();
    if (IN.groupIndex == 0)
    {
        LightCount = 0;
        ldsZMin = 0x7f7fffff;  // FLT_MAX as a uint
        ldsZMax = 0;
       
       /* LightIndexCounter[0] = 0;*/
    }
   
    GroupMemoryBarrierWithGroupSync();

    float3 frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3;
    {   // construct frustum for this tile
        uint pxm = BLOCK_SIZE * IN.groupID.x;
        uint pym = BLOCK_SIZE * IN.groupID.y;
        uint pxp = BLOCK_SIZE * (IN.groupID.x + 1);
        uint pyp = BLOCK_SIZE * (IN.groupID.y + 1);

        uint uWindowWidthEvenlyDivisibleByTileRes = BLOCK_SIZE * GetNumTilesX();
        uint uWindowHeightEvenlyDivisibleByTileRes = BLOCK_SIZE * GetNumTilesY();

        // four corners of the tile, clockwise from top-left
        float3 frustum0 = ConvertProjToView(float4(pxm / (float)uWindowWidthEvenlyDivisibleByTileRes * 2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pym) / (float)uWindowHeightEvenlyDivisibleByTileRes * 2.f - 1.f, 1.f, 1.f)).xyz;
        float3 frustum1 = ConvertProjToView(float4(pxp / (float)uWindowWidthEvenlyDivisibleByTileRes * 2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pym) / (float)uWindowHeightEvenlyDivisibleByTileRes * 2.f - 1.f, 1.f, 1.f)).xyz;
        float3 frustum2 = ConvertProjToView(float4(pxp / (float)uWindowWidthEvenlyDivisibleByTileRes * 2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pyp) / (float)uWindowHeightEvenlyDivisibleByTileRes * 2.f - 1.f, 1.f, 1.f)).xyz;
        float3 frustum3 = ConvertProjToView(float4(pxm / (float)uWindowWidthEvenlyDivisibleByTileRes * 2.f - 1.f, (uWindowHeightEvenlyDivisibleByTileRes - pyp) / (float)uWindowHeightEvenlyDivisibleByTileRes * 2.f - 1.f, 1.f, 1.f)).xyz;

        // create plane equations for the four sides of the frustum, 
        // with the positive half-space outside the frustum (and remember, 
        // view space is left handed, so use the left-hand rule to determine 
        // cross product direction)
        frustumEqn0 = CreatePlaneEquation(frustum0, frustum1);
        frustumEqn1 = CreatePlaneEquation(frustum1, frustum2);
        frustumEqn2 = CreatePlaneEquation(frustum2, frustum3);
        frustumEqn3 = CreatePlaneEquation(frustum3, frustum0);
    }

    

    // calculate the min and max depth for this tile, 
    // to form the front and back of the frustum

    float minZ = FLT_MAX;
    float maxZ = 0.f;

    CalculateMinMaxDepthInLds(IN.dispatchThreadID);

    GroupMemoryBarrierWithGroupSync();
 
    maxZ = asfloat(ldsZMax);
    minZ = asfloat(ldsZMin);

    // loop over the lights and do a sphere vs. frustum intersection test


    for (uint i = IN.groupIndex; i < g_lightCount.x; i += BLOCK_SIZE * BLOCK_SIZE)
        {

              Light light = SB_LIGHT[i];

              switch (light.type.x)
              {
              default:
              {
                  break;
              }
              case POINT_LIGHT:
              {
                  float3 center = mul(float4(light.position.xyz, 1), g_view).xyz;

                  float r = light.otherConstants.y;
                 
                  // test if sphere is intersecting or inside frustum
                  if (TestFrustumSides(center.xyz, r, frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3))
                  {
                      if (-center.z + minZ < r && center.z - maxZ < r)
                      {
                          AppendLight(i);
                      }
                      
                      
                  }
                 
                  break;
              }

              case SPOT_LIGHT:
              {
                  float3 center = mul(float4(light.position.xyz, 1), g_view).xyz;

                  float r = light.otherConstants.y;

                  // test if sphere is intersecting or inside frustum
                  if (TestFrustumSides(center.xyz, r, frustumEqn0, frustumEqn1, frustumEqn2, frustumEqn3))
                  {
                      if (-center.z + minZ < r && center.z - maxZ < r)
                      {
                          AppendLight(i);
                      }
                  }
                  break;
              }


              }
          }


    // Wait till all threads in group have caught up.
    GroupMemoryBarrierWithGroupSync();

   
    

    // Update global memory with visible light buffer.
    // First update the light grid (only thread 0 in group needs to do this)
    if (IN.groupIndex == 0)
    {
       
        InterlockedAdd(LightIndexCounter[0], LightCount, LightIndexStartOffset);
        lightGrid[IN.groupID.xy] = uint2(LightIndexStartOffset, LightCount);
       
        
    }
    
  

    GroupMemoryBarrierWithGroupSync();

    // Now update the light index list (all threads).

    for (i = IN.groupIndex; i < LightCount; i += BLOCK_SIZE * BLOCK_SIZE)
    {
        lightIndexList[LightIndexStartOffset + i] = LightList[i];
    }
 
    
    // Update the debug texture output.
    if (IN.groupThreadID.x == 0 || IN.groupThreadID.y == 0)
    {
        DebugTexture[texCoord] = float4(0, 0, 0, 0.9f);
    }
    else if (IN.groupThreadID.x == 1 || IN.groupThreadID.y == 1)
    {
        DebugTexture[texCoord] = float4(1, 1, 1, 0.5f);
    }
    else if (LightCount > 0)
    {
        float normalizedLightCount = LightCount / 50.0f;
        float4 lightCountHeatMapColor = LightCountHeatMap.SampleLevel(sampleClamp, float2(normalizedLightCount, 0), 0);
        DebugTexture[texCoord] = lightCountHeatMapColor;
       
    }
    else
    {
        
        DebugTexture[texCoord] = float4(0, 0, 0, 1);
        
       
    }



 
}




