#include "st_lighting.hlsli"

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
Texture2D third_texture : register(t2);
Texture2D depth_texture : register(t3);

// Light constants.
static float ambient = 1.0f;

cbuffer cb0 : register(b0)
{
	float4x4 inverse_vp;
	float4 eye;
	float4 light_position;
	float4 light_color;
	float2 light_properties;
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
	float4 albedo_sample = albedo_texture.Load(int3(input.position.xy, 0));
	float4 normal_sample = normal_texture.Load(int3(input.position.xy, 0));
	float4 third_sample = third_texture.Load(int3(input.position.xy, 0));
	float depth = depth_texture.Load(int3(input.position.xy, 0)).r;
	
	float3 albedo = albedo_sample.rgb;
	float metalness = albedo_sample.a;
	float3 normal = normal_sample.rgb * 2.0f - 1.0f;
	float roughness = normal_sample.a;
	float linear_roughness = roughness * roughness;
	float emissive = third_sample.r;

	float4 clip_position = float4(input.uv * 2.0f - 1.0f, depth, 1.0f);
	float4 world_position = mul(clip_position, inverse_vp);
	world_position /= world_position.w;
	
	float3 to_eye = normalize(eye.xyz - world_position.xyz);
	float3 to_light = light_position.xyz - world_position.xyz;
	float3 half_vector = normalize(to_eye + to_light);
	float dist_to_light = length(to_light);
	to_light = normalize(to_light);

	float n_dot_l = saturate(dot(normal, to_light));
	float n_dot_v = saturate(dot(normal, to_eye));
	float n_dot_h = saturate(dot(normal, half_vector));

	// Unpack light properties.
	float light_power = light_properties.x;

	float irradiance = light_falloff(light_power, dist_to_light);
	
	float3 diffuse_color = albedo * (1.0f - metalness);
	float3 specular_color = light_color.xyz;

	float3 diffuse_result = diffuse_color * diffuse_lambertian(n_dot_l) * irradiance;
	float3 specular_result = specular_color * specular_ggx(n_dot_v, n_dot_l, n_dot_h, metalness, linear_roughness) * irradiance;
	
	float3 lit_color = diffuse_result;
	lit_color += specular_result;
	lit_color += diffuse_color * ambient;
	
	lit_color += emissive * albedo_sample;
	
	return float4(lit_color, 1.0f);
};
