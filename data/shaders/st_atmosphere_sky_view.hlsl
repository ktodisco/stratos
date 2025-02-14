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

[[vk::binding(0, 2)]] cbuffer Constants : register(b0)
{
	st_atmosphere_constants constants;
}

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	// 1 in the z here in order to match the far depth value.
	result.position = float4(input.position.xy, 1.0f, 1.0f);
	result.uv = input.uv;
	
	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{
	const int num_samples = 32;
    
    const float atmo_radius = constants.radii_dims.y;
    const float planet_radius = constants.radii_dims.x;
    const float3 viewpoint = float3(0.0f, planet_radius, 0.0f);
    const float3 to_sun = normalize(-constants.light_dir);
	
	float3 view_dir = atmosphere_sky_encode_uv(input.uv);
	
	// Early out for anything below the horizon. This normally can't be seen, and will result in artifacts.
	if (view_dir.y < 0.0f)
		return 0.0f.xxxx;
	
	// Intersect the view direction with the sphere representing the atmosphere.
    float3 e = -viewpoint;
    float r2 = atmo_radius * atmo_radius;
    float e2 = dot(e, e);
    float a = dot(e, view_dir);
	
    float test = r2 - e2 + (a * a);
    float distance = a + sqrt(test);

    // We want to accumulate transmission for both rayleigh and mie.
    float3 rayleigh_atten = 0.0f.xxx;
    float3 mie_atten = 0.0f.xxx;
    
    float cos_theta = dot(view_dir, to_sun);
    float rayleigh_phase = (3.0f / (16.0f * PI)) * (1.0f + pow(cos_theta, 2.0f));
    const float g = 0.76;
    float mie_phase = (3.0f / (8.0f * PI)) * (((1.0f - g * g) * (1.0f + cos_theta * cos_theta)) / ((2.0f + g * g) * pow(1.0f + (g * g) - 2.0f * g * cos_theta, 1.5f)));
	
	// Integrate the scattering along the ray through to the atmosphere boundary.
    float sample_length = distance / float(num_samples);
    for (int i = 0; i < num_samples; ++i)
    {
        float3 sample_point = viewpoint + view_dir * (sample_length * (float(i) + 0.5f));

		float3 sample_down = -normalize(sample_point);

        // Sample incoming direct light transmission to this height.
        float height = sample_point.y - planet_radius;
        float h = height / (atmo_radius - planet_radius);
		float cos_sun_angle = dot(sample_down, to_sun);
        float y = cos_sun_angle * 0.5f + 0.5f;
		// The transmittance texture also contains lookups for going back to the surface,
		// so the sun below the horizon can't pick up those samples, hence the conditional.
		float3 t_sun = select(cos_sun_angle <= 0.0f,
			transmittance.Sample(transmittance_sampler, float2(h, y)).rgb,
			0.0f.xxx);
		
		// Sample transmission to the view point.
        float z = dot(sample_down, -view_dir) * 0.5f + 0.5f;
		float3 t_view = transmittance.Sample(transmittance_sampler, float2(h, z)).rgb;

        float p_r = exp(-height / constants.rayleigh_params.w) * sample_length;
        float p_m = exp(-height / constants.mie_params.w) * sample_length;

        rayleigh_atten += (t_sun * t_view) * p_r;
        mie_atten += (t_sun * t_view) * p_m;
    }
    
    float3 result = rayleigh_atten * constants.rayleigh_params.xyz * rayleigh_phase;
    result += mie_atten * constants.mie_params.xyz * mie_phase;
	
	return float4(result, 1.0f);
}