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

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	column_major float4x4 mvp;
	float3 color;
}

[[vk::binding(0, 0)]] Texture2D font_texture : register(t0);
[[vk::binding(0, 1)]] SamplerState font_sampler : register(s0);

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = mul(float4(input.position, 1.0f), mvp);
	result.uv = input.uv;

	return result;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	float tex_alpha = font_texture.Sample(font_sampler, input.uv).r;
	
	return float4(color, tex_alpha);
}
