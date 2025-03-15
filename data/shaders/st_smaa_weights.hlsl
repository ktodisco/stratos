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

[[vk::binding(0, 0)]] Texture2D edges_tex : register(t0);
[[vk::binding(1, 0)]] Texture2D area_tex : register(t1);
[[vk::binding(2, 0)]] Texture2D search_tex : register(t2);
[[vk::binding(0, 1)]] SamplerState LinearSampler : register(s0);
[[vk::binding(1, 1)]] SamplerState PointSampler : register(s1);
[[vk::binding(2, 1)]] SamplerState search_sampler : register(s2);

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	float4 dimensions;
};

#include "st_smaa.hlsli"

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	result.position = float4(input.position.xy, 0.0f, 1.0f);
	result.uv = input.uv;
	
	float4 offset[3];
	float2 pixcoord;
	SMAABlendingWeightCalculationVS(input.uv, pixcoord, offset);
	result.offset0 = offset[0];
	result.offset1 = offset[1];
	result.offset2 = offset[2];
	
	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{
	float4 offset[3] = { input.offset0, input.offset1, input.offset2 };
	float2 pixcoord = input.uv * dimensions.xy;
	float4 result = SMAABlendingWeightCalculationPS(
		input.uv,
		pixcoord,
		offset,
		edges_tex,
		area_tex,
		search_tex,
		0.0.xxxx);
	
	return result;
};