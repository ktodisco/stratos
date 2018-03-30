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

// Light constants.
static float3 light_dir = normalize(float3(-1.0, -1.0, -1.0));
static float3 ambient = float3(0.2, 0.2, 0.2);
static float3 diffuse = float3(0.5, 0.5, 0.5);
static float3 specular = float3(0.5, 0.5, 0.5);

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	float2 texcoord_base = float2(0.5f, -0.5f);
	result.uv = input.position.xy * texcoord_base + texcoord_base;
	
	result.position = float4(input.position.xy, 0.0f, 1.0f);
	
	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 albedo = albedo_texture.Load(int3(input.position.xy, 0));
	float3 normal = normal_texture.Load(int3(input.position.xy, 0));
	
	float3 to_light = -light_dir;
	
	float3 diffuse_color = diffuse * max(dot(to_light, normal), 0) * albedo;
	float3 lit_color = diffuse_color + (ambient * albedo);
	
	return float4(lit_color, 1.0f);
};
