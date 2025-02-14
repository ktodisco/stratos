#include "st_math.hlsli"
#include "st_atmosphere.hlsli"

[[vk::binding(0, 2)]] cbuffer Constants : register(b0)
{
	st_atmosphere_constants constants;
}

[[vk::binding(0, 3)]]
[[vk::image_format("rgba8")]]
RWTexture2D<float4> output : register(u0);

[numthreads(32, 32, 1)]
void cs_main(uint3 threadID : SV_DispatchThreadID)
{
	float2 uv = threadID.xy * constants.radii_dims.zw;
	
	const float rg = constants.radii_dims.x;
	const float rt = constants.radii_dims.y;
	
	// Depending on the view angle, must calculate the distance to either:
	//  - the atmosphere boundary, or
	//  - the ground.
	// The first one to check is the ground. If there's an intersection then
	// the solution sought it t = a - f.
	// If there's no intersection with the ground, then we need the intersection
	// with the atmosphere boundary, which is t = a + f.

	// First, we need the view direction. If 0.0 is straight up and 1.0 straight
	// down, then x is given by the sin and y is given by cos.
	float2 dir = float2(sin(uv.y * PI), cos(uv.y * PI));

	// We want to find the distance the ray travels through the medium.
	float distance = 0.0f;

	// The current height is given by uv.x.
	float height = rg + ((rt - rg) * uv.x);

	// First, check for an intersection with the planet.
	// Ray-circle intersection is given by the formula:
	//     f = sqrt(r^2 - e^2 + a^2)
	// where r is the radius of the circle, e is the line from the ray origin
	// to the circle center, and a is e projected onto d, or d.e.
	float2 e = float2(0.0f, -height);
	float r2 = rg * rg;
	float e2 = dot(e, e);
	float a = dot(e, dir);

	float test = r2 - e2 + (a * a);
    
	// Either the vector is pointing away from the planet, or there is not intersection.
	if (test < 0.0f || dir.y > 0.0f)
	{
		// Test against the atmophere instead. There will always be an intersection here.
		r2 = rt * rt;
		test = r2 - e2 + (a * a);
		distance = a + sqrt(test);
	}
	else
	{
		distance = a - sqrt(test);
	}

	// Calculate the sample length.
	const int num_samples = 100;
	float sample_length = distance / float(num_samples);
	float optical_depth_r = 0.0f;
	float optical_depth_m = 0.0f;
	float optical_depth_o = 0.0f;
	float2 orig = float2(0.0f, height);
	for (int i = 0; i < num_samples; ++i)
	{
		float2 sample_point = orig + (dir * (sample_length * (float(i) + 0.5f)));
		float sample_height = (length(sample_point) - rg);
		optical_depth_r += exp(-sample_height / constants.rayleigh_params.w) * sample_length;
		optical_depth_m += exp(-sample_height / constants.mie_params.w) * sample_length;
		optical_depth_o += max(0.0f, 1.0f - abs(sample_height - constants.ozone_params.w) / 15000.0f);
	}
	
	// Note that this uses a hybrid of the Bruneton method (the multiplication by 1.1 to
	// approximate Mie absorbtion) and the Hillaire method (ozone absorbtion). A more
	// accurate model would include separate figures for scattering and absorbtion.
	float3 tau = optical_depth_r * constants.rayleigh_params.xyz;
	tau += 1.1f * optical_depth_m * constants.mie_params.xyz;
	tau += optical_depth_o * constants.ozone_params.xyz;
	float3 transmittance = exp(-tau);
    
	// This is the amount of light scattered before it reaches this point from the
	// view direction.
	output[threadID.xy] = float4(transmittance, 1.0f);
}