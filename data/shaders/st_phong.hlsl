struct vs_input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float3 normal : TEXCOORD0;
};

cbuffer cb0 : register(b0)
{
	float4x4 mvp;
}

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = mul(float4(input.position, 1.0f), mvp);
	result.normal = input.normal;

	return result;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
