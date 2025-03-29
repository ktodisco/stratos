#include "st_gamma_correction.hlsli"

struct vs_input
{
	float3 position : POSITION;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

[[vk::binding(0, 0)]] Texture2D source : register(t0);
[[vk::binding(0, 1)]] SamplerState source_sampler : register(s0);

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	float is_hdr;
	
	float pad0;
	float pad1;
	float pad2;
};

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	result.position = float4(input.position.xy, 0.0f, 1.0f);
	result.uv = input.uv;
	
	return result;
};

float3 convert_709_to_2020(float3 x)
{
	const float3x3 conversion =
	{
		{ 0.67420192f, 0.06909549f, 0.01639371f },
		{ 0.32929179f, 0.91954428f, 0.08802816f },
		{ 0.0433061f, 0.01136023f, 0.89557813f },
	};

	return mul(x, conversion);
}

float3 pq_encode(float3 x)
{
	const float k_ref_white = 10000.0f / 80.0f;
	x /= k_ref_white;
	
	const float m1 = 0.1593017578125f;
	const float m2 = 78.84375f;
	const float c1 = 0.8359375f;
	const float c2 = 18.8515625f;
	const float c3 = 18.6875f;
	
	float3 num = c1 + c2 * pow(x, m1);
	float3 den = 1.0f + c3 * pow(x, m1);
	
	return pow(num / den, m2);
}

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 result = source.Sample(source_sampler, input.uv).rgb;
	
	if (is_hdr)
	{
		result.rgb = convert_709_to_2020(result.rgb);
		result.rgb = pq_encode(result.rgb);
	}
	else
		result.rgb = linear_to_srgb(result.rgb);
	
	return float4(result, 1.0f);
};