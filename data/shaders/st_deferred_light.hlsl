#include "st_encoding.hlsli"
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

[[vk::binding(0, 0)]] Texture2D albedo_texture : register(t0);
[[vk::binding(1, 0)]] Texture2D normal_texture : register(t1);
[[vk::binding(2, 0)]] Texture2D third_texture : register(t2);
[[vk::binding(3, 0)]] Texture2D depth_texture : register(t3);
[[vk::binding(4, 0)]] Texture2D directional_shadow_map : register(t4);
[[vk::binding(4, 1)]] SamplerState directional_shadow_sampler : register(s4);

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	column_major float4x4 inverse_vp;
	float4 eye;
	float4 depth_reconstruction;

	float4 sun_direction_power;
	float4 sun_color;
	column_major float4x4 sun_vp;
	float4 sun_shadow_dim;
	
	float exposure_value;
	
	float pad0;
	float pad1;
	float pad2;
}

struct st_sphere_light
{
	float4 position_power;
	float4 color_radius;
};

[[vk::binding(0, 3)]] StructuredBuffer<st_sphere_light> light_buffer : register(t8);

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	float2 texcoord_base = float2(0.5f, 0.5f);
	result.uv = input.position.xy * texcoord_base + texcoord_base;

	result.position = float4(input.position.xy, 0.0f, 1.0f);
	
	return result;
};

float directional_light_shadow(in float4 world_position)
{
	float4 projection = mul(world_position, sun_vp);
	projection.xyz /= projection.w;
	float2 center_uv = (projection.xy + 1.0f) * depth_reconstruction.zw;
	
	float occlusion = 0.0f;
	for (int x = -1; x <= 1; ++x)
    {
		for (int y = -1; y <= 1; ++y)
        {
			float2 offset = float2(x, y) * sun_shadow_dim.zw;
			float shadow_depth = directional_shadow_map.Sample(directional_shadow_sampler, center_uv + offset).r * depth_reconstruction.x - depth_reconstruction.y;
			if (shadow_depth < projection.z - 0.001f)
            {
				occlusion += 1.0f;	
            }
        }
    }
	
	float visibility = 1.0f - (occlusion / 9.0f);
	
	return visibility;
}

float3 evaluate_directional_light(
	in float3 light_direction,
	in float3 light_color,
	in float illuminance,
	in float3 albedo,
	in float3 normal,
	in float metalness,
	in float linear_roughness,
	in float3 to_eye,
	in float4 world_position)
{
	float3 to_light = -light_direction;

	float n_dot_l_raw = dot(normal, to_light);
	float n_dot_l = saturate(n_dot_l_raw);
	float n_dot_v = saturate(dot(normal, to_eye));
	
	float3 diffuse_color = albedo * (1.0f - metalness) * light_color.xyz;
	float3 specular_color = metalness * light_color.xyz;
	
	float visibility_term = directional_light_shadow(world_position);

	// Division by pi plays the part of the lambertian diffuse.
	float3 diffuse_result = diffuse_color * illuminance * n_dot_l * visibility_term / k_pi;

	float3 half_vector = normalize(to_eye + to_light);
	float n_dot_h = saturate(dot(normal, half_vector));
	float3 specular_result = specular_color *
		specular_ggx(
			albedo,
			n_dot_v,
			n_dot_l,
			n_dot_h,
			metalness,
			linear_roughness) * illuminance;
	
	return diffuse_result + specular_result;
}

float3 evaluate_sphere_light(
	in st_sphere_light light,
	in float3 albedo,
	in float3 normal,
	in float metalness,
	in float linear_roughness,
	in float3 to_eye,
	in float4 world_position)
{
	// Unpack light properties.
	float light_power = light.position_power.w;
	float light_radius = light.color_radius.w;

	float3 to_light = light.position_power.xyz - world_position.xyz;
	float dist_to_light_center = length(to_light);
	float dist_to_light = dist_to_light_center - light_radius;
	to_light = normalize(to_light);

	float n_dot_l_raw = dot(normal, to_light);
	float n_dot_l = saturate(n_dot_l_raw);
	float n_dot_v = saturate(dot(normal, to_eye));

	float illuminance = light_falloff(light_power, dist_to_light);
	
	float3 diffuse_color = albedo * (1.0f - metalness) * light.color_radius.xyz;
	float3 specular_color = metalness * light.color_radius.xyz;

	float visibility_term = get_sphere_visibility(to_light, dist_to_light_center, n_dot_l_raw, light_radius);

	// Division by pi plays the part of the lambertian diffuse.
	float3 diffuse_result = diffuse_color * illuminance * visibility_term / k_pi;

	float3 reflection = reflect(-to_eye, normal);
	float3 center_to_ray = dot(to_light, reflection) * reflection - to_light;
	float3 representative_point = to_light + center_to_ray * saturate(light_radius / length(center_to_ray));
	float3 half_vector = normalize(to_eye + normalize(representative_point));
	float n_dot_h = saturate(dot(normal, half_vector));
	float3 specular_result = specular_color *
		specular_ggx(
			albedo,
			n_dot_v,
			n_dot_l,
			n_dot_h,
			metalness,
			linear_roughness) * illuminance;
	// Cap the specular result to the light power.
	specular_result = min(specular_result, light_power.xxx);
	
	return diffuse_result + specular_result;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	float4 albedo_sample = albedo_texture.Load(int3(input.position.xy, 0));
	float4 normal_sample = normal_texture.Load(int3(input.position.xy, 0));
	float4 third_sample = third_texture.Load(int3(input.position.xy, 0));
	float depth = depth_texture.Load(int3(input.position.xy, 0)).r * depth_reconstruction.x - depth_reconstruction.y;
	
	float3 albedo = albedo_sample.rgb;
	float metalness = albedo_sample.a;
	float3 normal = oct_decode(normal_sample.rg);
	float roughness = third_sample.r;
	float linear_roughness = roughness * roughness;
	float emissive = third_sample.g;

	float4 clip_position = float4(input.uv * 2.0f - 1.0f, depth, 1.0f);
	float4 world_position = mul(clip_position, inverse_vp);
	world_position /= world_position.w;
	
	float3 to_eye = normalize(eye.xyz - world_position.xyz);

	float3 lit_color = evaluate_directional_light(
		sun_direction_power.xyz,
		sun_color.xyz,
		sun_direction_power.w,
		albedo,
		normal,
		metalness,
		linear_roughness,
		to_eye,
		world_position);
	
	lit_color += evaluate_sphere_light(
		light_buffer[0],
		albedo,
		normal,
		metalness,
		linear_roughness,
		to_eye,
		world_position);
	
	lit_color += emissive * albedo_sample;
	lit_color *= exposure_value;

	return float4(lit_color, 1.0f);
};
