struct vs_input
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float3 color : COLOR0;
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
	result.color = input.color;

	return result;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	return float4(constant_color * input.color, 1.0f);
}
