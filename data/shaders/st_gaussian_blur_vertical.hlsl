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
[[vk::binding(0, 1)]] SamplerState tex_sampler : register(s0);

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
	float weights[9] =
    {
		0.02f, 0.04f, 0.08f, 0.16f, 0.4f, 0.16f, 0.08f, 0.04f, 0.02f
    };
	
	float3 result = float3(0.0f, 0.0f, 0.0f);
	for (int y = -4; y <= 4; ++y)
    {
		float2 offset = float2(0.0f, y * source_dim.w);
		float3 source = tex.Sample(tex_sampler, input.uv + offset).rgb;
		result += source * weights[y + 4];
    }

	return float4(result, 1.0f);
};
