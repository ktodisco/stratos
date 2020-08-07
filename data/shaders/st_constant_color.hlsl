struct vs_input
{
	float3 position : POSITION;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
};

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	float4x4 mvp;
	float3 constant_color;
}

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = mul(float4(input.position, 1.0f), mvp);

	return result;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	return float4(constant_color, 1.0f);
}
