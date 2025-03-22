#include "st_math.hlsli"
#include "st_atmosphere.hlsli"

struct vs_input
{
	float3 position : POSITION;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

[[vk::binding(0, 0)]] Texture2D transmittance : register(t0);
[[vk::binding(0, 1)]] SamplerState transmittance_sampler : register(s0);
[[vk::binding(1, 0)]] Texture2D sky_view : register(t1);
[[vk::binding(1, 1)]] SamplerState sky_view_sampler: register(s1);
	
[[vk::binding(0, 2)]] cbuffer Constants : register(b0)
{
	st_atmosphere_constants constants;
}

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	// 1 in the z here in order to match the far depth value.
	result.position = float4(input.position.xy, 1.0f, 1.0f);
	
	// Manual UVs here as the view direction reconstruction based on UV is
	// independent of backend.
	float2 texcoord_base = float2(0.5f, 0.5f);
	result.uv = input.position.xy * texcoord_base + texcoord_base;
	
	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{    
    const float atmo_radius = constants.radii_dims.y;
    const float planet_radius = constants.radii_dims.x;
    const float3 viewpoint = float3(0.0f, planet_radius, 0.0f);
    const float3 to_sun = normalize(-constants.light_dir);
	
	// Extract the view direction.
	float3 view_dir_ndc = float3(input.uv.x * 2.0f - 1.0f, input.uv.y * 2.0f - 1.0f, -1.0f);
	float3 view_dir_v = normalize(mul(float4(view_dir_ndc, 1.0f), constants.inverse_p).xyz);
	float3 view_dir = normalize(mul(float4(view_dir_v, 0.0f), constants.inverse_v).xyz);
	
	// Early out for anything below the horizon. This normally can't be seen, and will result in artifacts.
	if (view_dir.y < 0.0f)
		return 0.0f.xxxx;
	
	float2 uv = atmosphere_sky_decode_uv(view_dir);
	float3 result = sky_view.Sample(sky_view_sampler, uv) * constants.sun_power;
	
	// If looking at the sun, output the max.
	if (dot(view_dir, to_sun) > 0.9995f)
	{
		float height = viewpoint.y - planet_radius;
		float h = height / (atmo_radius - planet_radius);
		float y = dot(-normalize(viewpoint), to_sun) * 0.5f + 0.5f;
		float3 t_sun = transmittance.Sample(transmittance_sampler, float2(h, y)).rgb;
		result = float4(constants.sun_power * t_sun, 1.0f);
	}
	
	// This value is temporary but deliberate. It's calculated as the sun power - 120,000 lux -
	// pre-exposed by the sunny 16 rule but then multiplied by the reference white 80 found in
	// tonemapping.
	// Pre-expsosure will be replaced by a real camera exposure calculation, and the reference
	// white multiply will be corrected with it.
	return float4(result * constants.exposure_value, 1.0f);
}