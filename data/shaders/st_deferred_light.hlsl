struct vs_input
{
	float3 position : POSITION;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

Texture2D albedo_texture : register(t0);
Texture2D normal_texture : register(t1);
Texture2D depth_texture : register(t2);

// Light constants.
static float3 ambient = float3(0.2, 0.2, 0.2);

cbuffer cb0 : register(b0)
{
	float4x4 inverse_vp;
	float3 light_position;
	float3 light_color;
	float light_power;
}

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	float2 texcoord_base = float2(0.5f, 0.5f);
	result.uv = input.position.xy * texcoord_base + texcoord_base;
	
	result.position = float4(input.position.xy, 0.0f, 1.0f);
	
	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 albedo = albedo_texture.Load(int3(input.position.xy, 0)).rgb;
	float3 normal = normal_texture.Load(int3(input.position.xy, 0)).rgb;
	float depth = depth_texture.Load(int3(input.position.xy, 0)).r;

	float4 clip_position = float4(input.uv * 2.0f - 1.0f, depth, 1.0f);
	float4 world_position = mul(clip_position, inverse_vp);
	world_position /= world_position.w;
	
	float3 to_light = normalize(light_position - world_position.xyz);
	
	float3 diffuse_color = max(dot(to_light, normal), 0) * albedo * light_color;
	float3 lit_color = diffuse_color + (ambient * albedo);
	
	return float4(lit_color, 1.0f);
};
