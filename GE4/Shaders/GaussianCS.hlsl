#include "Common.hlsli"

RWTexture2D<float4> gOutputH: register(u4);
RWTexture2D<float4> gOutputV: register(u5);
Texture2D gInputH : register(t5);
Texture2D gInputV : register(t1);

#define Num 256
static const int gBlurRadius = 5;
#define CacheSize (Num + 2 * gBlurRadius)
static const float gWeights[11] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f };
groupshared float4 gCache[CacheSize];



[numthreads(Num, 1, 1)]
void HorzBlurCS(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{

	
	//
	// Fill local thread storage to reduce bandwidth.  To blur 
	// N pixels, we will need to load N + 2*BlurRadius pixels
	// due to the blur radius.
	//

	// This thread group runs N threads.  To get the extra 2*BlurRadius pixels, 
	// have 2*BlurRadius threads sample an extra pixel.
	if (groupThreadID.x < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int x = max(dispatchThreadID.x - gBlurRadius, 0);
		gCache[groupThreadID.x] = gInputH[int2(x, dispatchThreadID.y)];
	}
	if (groupThreadID.x >= Num - gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int x = min(dispatchThreadID.x + gBlurRadius, gInputH.Length.x - 1);
		gCache[groupThreadID.x + 2 * gBlurRadius] = gInputH[int2(x, dispatchThreadID.y)];
	}

	// Clamp out of bound samples that occur at image borders.
	gCache[groupThreadID.x + gBlurRadius] = gInputH[min(dispatchThreadID.xy, gInputH.Length.xy - 1)];

	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();

	//
	// Now blur each pixel.
	//

	float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + gBlurRadius + i;

		blurColor += gWeights[i + gBlurRadius] * gCache[k];
	}

	gOutputH[dispatchThreadID.xy] = blurColor;
}

[numthreads(1, Num, 1)]
void VertBlurCS(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
	//
	// Fill local thread storage to reduce bandwidth.  To blur 
	// N pixels, we will need to load N + 2*BlurRadius pixels
	// due to the blur radius.
	//

	// This thread group runs N threads.  To get the extra 2*BlurRadius pixels, 
	// have 2*BlurRadius threads sample an extra pixel.
	if (groupThreadID.y < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = max(dispatchThreadID.y - gBlurRadius, 0);
		gCache[groupThreadID.y] = gInputV[int2(dispatchThreadID.x, y)];
	}
	if (groupThreadID.y >= Num - gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = min(dispatchThreadID.y + gBlurRadius, gInputV.Length.y - 1);
		gCache[groupThreadID.y + 2 * gBlurRadius] = gInputV[int2(dispatchThreadID.x, y)];
	}

	// Clamp out of bound samples that occur at image borders.
	gCache[groupThreadID.y + gBlurRadius] = gInputV[min(dispatchThreadID.xy, gInputV.Length.xy - 1)];


	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();

	//
	// Now blur each pixel.
	//

	float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;

		blurColor += gWeights[i + gBlurRadius] * gCache[k];
	}

	gOutputV[dispatchThreadID.xy] = blurColor;
}






