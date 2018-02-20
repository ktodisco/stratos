struct vs_input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

cbuffer cb0 : register(b0)
{
	float4x4 mvp;
}

Texture2D diffuse_texture : register(t0);
SamplerState diffuse_sampler : register(s0);

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = mul(float4(input.position, 1.0f), mvp);
	result.uv = input.uv;

	return result;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 tex_color = diffuse_texture.Sample(diffuse_sampler, input.uv).rgb;
	
	return float4(tex_color, 1.0f);
}
