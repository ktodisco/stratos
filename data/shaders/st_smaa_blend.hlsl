struct vs_input
{
	float3 position : POSITION;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 offset : TEXCOORD1;
};

[[vk::binding(0, 0)]] Texture2D source_tex : register(t0);
[[vk::binding(1, 0)]] Texture2D weights_tex : register(t1);
[[vk::binding(0, 1)]] SamplerState LinearSampler : register(s0);
[[vk::binding(1, 1)]] SamplerState PointSampler : register(s1);

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
	
	SMAANeighborhoodBlendingVS(input.uv, result.offset);
	
	return result;
};

float3 gamma_correct(float3 x)
{
	float3 low = x * 12.92f;
	float3 high = (pow(abs(x), 1.0f / 2.4f) * 1.055f) - 0.055f;
	float3 srgb = select(x <= 0.0031308f, low, high);
	return srgb;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	float4 result = SMAANeighborhoodBlendingPS(
		input.uv,
		input.offset,
		source_tex,
		weights_tex);
	
	result.rgb = gamma_correct(result.rgb);
	
	return result;
};