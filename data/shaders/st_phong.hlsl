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
	float3 normal : TEXCOORD0;
};

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
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
	// Light constants.
	const float3 light_dir = normalize(float3(-1.0f, -1.0f, -1.0f));
	const float3 ambient = float3(0.2f, 0.2f, 0.2f);
	const float3 diffuse = float3(0.5f, 0.5f, 0.5f);
	const float3 specular = float3(0.5f, 0.5f, 0.5f);
	
	float3 to_light = -light_dir;
	
	float3 diffuse_color = diffuse * max(dot(to_light, input.normal), 0);
	float3 lit_color = ambient + diffuse_color;
	
	return float4(lit_color, 1.0f);
}
