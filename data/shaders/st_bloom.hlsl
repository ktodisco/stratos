struct vs_input
{
	float3 position : POSITION;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

[[vk::binding(0, 0)]] Texture2D tex : register(t0);
[[vk::binding(0, 1)]] SamplerState tex_sampler : register(s0);

ps_input vs_main(vs_input input)
{
	ps_input result;

	float2 texcoord_base = float2(0.5f, 0.5f);
	result.uv = input.position.xy * texcoord_base + texcoord_base;
	result.uv.y = 1.0f - result.uv.y;

	result.position = float4(input.position.xy, 0.0f, 1.0f);

	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 source = tex.Sample(tex_sampler, input.uv).rgb;
	float3 filtered = source > 4.0f ? source : 0.0f;

	return float4(filtered, 1.0f);
};
