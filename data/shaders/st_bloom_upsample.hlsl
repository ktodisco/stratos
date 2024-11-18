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

[[vk::binding(0, 0)]] Texture2D blur : register(t0);
[[vk::binding(1, 0)]] Texture2D step : register(t1);
[[vk::binding(0, 1)]] SamplerState blur_sampler : register(s0);
[[vk::binding(1, 1)]] SamplerState step_sampler : register(s1);

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	float4 source_dim;
}

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = float4(input.position.xy, 0.0f, 1.0f);
	result.uv = input.uv;

	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{
	float2 offsets[9] =
    {
		float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f),
		float2(-1.0f,  0.0f), float2(0.0f,  0.0f), float2(1.0f,  0.0f),
		float2(-1.0f,  1.0f), float2(0.0f,  1.0f), float2(1.0f,  1.0f)
    };
	float weights[9] =
    {
		1.0f, 2.0f, 1.0f,
		2.0f, 4.0f, 2.0f,
		1.0f, 2.0f, 1.0f
    };
	
	float3 blur_source = 0.0f.xxx;
	for (int i = 0; i < 9; ++i)
    {
		blur_source += blur.Sample(blur_sampler, input.uv + (offsets[i] * source_dim.zw)).rgb * weights[i];
    }
	blur_source /= 16.0f;
	
	float3 step_source = step.Sample(step_sampler, input.uv).rgb;
	
	const float blur_weight = 0.8f;
	return float4(blur_source * blur_weight + step_source * (1.0f - blur_weight), 1.0f);
};
