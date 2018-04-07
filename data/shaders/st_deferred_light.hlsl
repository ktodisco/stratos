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
static float s_pi = 3.14159f;
static float3 ambient = float3(0.2, 0.2, 0.2);

cbuffer cb0 : register(b0)
{
	float4x4 inverse_vp;
	float4 eye;
	float4 light_position;
	float4 light_color;
	float light_power;
}

float light_falloff(float power, float distance)
{
	return 1.0f / ((distance * distance) / 100.0f);
}

float3 diffuse_lambertian(float3 albedo, float3 n, float3 l)
{
	return (albedo / s_pi) * saturate(dot(n, l));
}

float3 specular_phong(float3 specular, float3 n, float3 v, float3 l, float g)
{
	float3 r = reflect(-l, n);
	float a = saturate(dot(r, v));
	return specular * pow(a, ((1.0f - g) * 50.0f) / 4.0f);
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
	float depth = depth_texture.Load(int3(input.position.xy, 0)).r;
	
	float3 albedo = albedo_sample.rgb;
	float metalness = albedo_sample.a;
	float3 normal = normal_sample.rgb * 2.0f - 1.0f;
	float gloss = normal_sample.a;

	float4 clip_position = float4(input.uv * 2.0f - 1.0f, depth, 1.0f);
	float4 world_position = mul(clip_position, inverse_vp);
	world_position /= world_position.w;
	
	float3 to_eye = normalize(eye.xyz - world_position.xyz);
	float3 to_light = light_position.xyz - world_position.xyz;
	float dist_to_light = length(to_light);
	to_light = normalize(to_light);
	
	float3 diffuse_color = albedo * (1.0f - metalness);
	float3 specular_color = light_color.xyz * metalness;
	
	float3 lit_color = diffuse_lambertian(diffuse_color, normal, to_light) * light_falloff(light_power, dist_to_light);
	lit_color += specular_phong(specular_color, normal, to_eye, to_light, gloss) * light_falloff(light_power, dist_to_light);
	lit_color += diffuse_color * ambient;
	
	return float4(lit_color, 1.0f);
};
