#include "Common.hlsli"


[maxvertexcount(18)]
void main(
	triangle input_PS input[3],
	inout TriangleStream< input_PS > output
)
{
	for (int i = 0; i < 6; i++)
	{
		input_PS element;
		element.index = i;


		for (uint j = 0; j < 3; j++)
		{
			
			float4 pos =  mul(float4(input[j].worldPos, 1), g_vpPL[i]);
			element.shadowPos = pos;
			element.worldPos = input[j].worldPos;
			element.uv = input[j].uv;
			element.normal = input[j].normal;
			element.tangent = input[j].tangent;
			element.biNormal = input[j].biNormal;
			element.pos = pos;
			element.posView = input[j].posView;
			element.normalView = input[j].normalView;
			output.Append(element);
		}

		output.RestartStrip();
	}


}


[maxvertexcount(18)]
void main2(
	triangle INPUT_PS_SKY input[3],
	inout TriangleStream< INPUT_PS_SKY > output
)
{
	for (int i = 0; i < 6; i++)
	{
		INPUT_PS_SKY element;
		element.index = i;


		for (uint j = 0; j < 3; j++)
		{
			
			element.pos = mul(float4(input[j].posW, 1), g_vpPL[i]).xyww;
			element.uv = input[j].uv;
			element.posW = input[j].posW;
			output.Append(element);
		}

		output.RestartStrip();
	}


}

[maxvertexcount(18)]
void main3(
	triangle PS_INPUT_TERRAIN input[3],
	inout TriangleStream< PS_INPUT_TERRAIN > output
)
{
	for (int i = 0; i < 6; i++)
	{
		PS_INPUT_TERRAIN element;
		element.index = i;


		for (uint j = 0; j < 3; j++)
		{

			element.pos = mul(float4(input[j].worldPos, 1), g_vpPL[i]).xyww;
			element.uv = input[j].uv;
			element.worldPos = input[j].worldPos;
			element.tileUV = input[j].tileUV;
			element.shadowPos = input[j].shadowPos;
			element.bounds = input[j].bounds;
			element.posView = input[j].posView;
			output.Append(element);
		}

		output.RestartStrip();
	}


}

