#include "Common.hlsli"

psQuadInput main1(uint vI : SV_VERTEXID)
{

    psQuadInput output;

    float2 texcoord = float2(vI >> 1, vI & 1);
    output.pos = float4((texcoord.x - 0.5f) * 2, -(texcoord.y - 0.5f) * 2, 0, 1);

    output.uv = texcoord;


    return output;
}

psQuadInput main(INPUT_VS input)
{

    psQuadInput output;

    
    output.pos = float4(input.pos, 1);

    output.uv = input.uv;


    return output;
}