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

[[vk::binding(0, 0)]] Texture2D tex : register(t0);
[[vk::binding(1, 0)]] Texture2D bloom : register(t1);
[[vk::binding(0, 1)]] SamplerState tex_sampler : register(s0);
[[vk::binding(1, 1)]] SamplerState bloom_sampler : register(s1);

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	result.position = float4(input.position.xy, 0.0f, 1.0f);
	result.uv = input.uv;
	
	return result;
};

float3 aces_film(float3 x)
{
	// Tonemap down to the reference white level of 80 nits.
	const float k_ref_white = 80.0f;
	x /= k_ref_white;
	
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
	float3 result = saturate((x * (a * x + b)) / (x * (c * x + d) + e));

	return result;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 source = tex.Sample(tex_sampler, input.uv).rgb;
	float3 bloomed = source + bloom.Sample(bloom_sampler, input.uv).rgb;
	float3 tonemap = aces_film(bloomed);
	
	return float4(tonemap, 1.0f);
};