struct vs_input
{
	float3 position : POSITION;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 offset0 : TEXCOORD1;
	float4 offset1 : TEXCOORD2;
	float4 offset2 : TEXCOORD3;
};

[[vk::binding(0, 0)]] Texture2D tex : register(t0);
[[vk::binding(0, 1)]] SamplerState PointSampler : register(s0);

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	float4 dimensions;
};

#define LinearSampler PointSampler
#include "st_smaa.hlsli"

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	result.position = float4(input.position.xy, 0.0f, 1.0f);
	result.uv = input.uv;
	
	float4 offset[3];
	SMAAEdgeDetectionVS(input.uv, offset);
	result.offset0 = offset[0];
	result.offset1 = offset[1];
	result.offset2 = offset[2];
	
	return result;
};

float2 ps_main(ps_input input) : SV_TARGET
{
	float4 offset[3] = { input.offset0, input.offset1, input.offset2 };
	float2 result = SMAALumaEdgeDetectionPS(input.uv, offset, tex);
	
	return result;
};