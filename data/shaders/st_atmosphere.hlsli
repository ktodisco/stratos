#ifndef __ST_ATMOSPHERE_HLSLI__
#define __ST_ATMOSPHERE_HLSLI__

#include "st_math.hlsli"

/*
** Atmosphere rendering is adopted from the following papers:
**    http://www.klayge.org/material/4_0/Atmospheric/Precomputed%20Atmospheric%20Scattering.pdf
**    https://sebh.github.io/publications/egsr2020.pdf
** It is a hybrid of the two, with the goal being simplicity and clarity over true accuracy.
**
** Some math is also borrowed from:
**    https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-16-accurate-atmospheric-scattering
*/

struct st_atmosphere_constants
{
	float4 radii_dims;
	float4 rayleigh_params;
	float4 mie_params;
	float4 ozone_params;
	float3 light_dir;
	float pad0;
	float2 light_angles;
	float pad1;
	float pad2;
	column_major float4x4 inverse_p;
	column_major float4x4 inverse_v;
};

float3 atmosphere_sky_encode_uv(in float2 uv)
{
	// Parameterization: u is uniform from 0 to 2*pi.
	// v is quadratic to get more pixels at the horizon.
	float u = uv.x * PI_2;
	float l = uv.y < 0.5f ? 1.0f - 2.0f * uv.y : uv.y * 2.0f - 1.0f;
	float v = l * l * PI_OVER_2;
	
	// Assemble the view direction.
	float3 view_dir = float3(cos(u) * cos(v), sin(v), sin(u) * cos(v));	
	
	return view_dir;
}

float2 atmosphere_sky_decode_uv(in float3 view_dir)
{
	float3 view_xz = normalize(float3(view_dir.x, 0.0f, view_dir.z));
	float sin_theta = dot(float3(0.0f, 0.0f, -1.0f), view_xz);
	float cos_theta = dot(float3(-1.0f, 0.0f, 0.0f), view_xz);
	float azimuth = atan2(sin_theta, cos_theta) + PI;
	float angle = -acos(dot(float3(0.0f, 1.0f, 0.0f), view_dir)) + PI_OVER_2;
	
	// Quadratic mapping at horizon.
	float v = 0.5f + 0.5f * sign(angle) * sqrt(abs(angle / PI_OVER_2));
	float u = azimuth / PI_2;
	
	return float2(u, v);
}

#endif