// Most of this code is borrowed from https://learnopengl.com/PBR/Theory
// Some formulas are also used by UE4.

static const float k_pi = 3.1415926f;

float3 fresnel(float3 f0, float f90, float h_dot_v)
{
	// Schlick's approximation.
	float x = 1.0f - h_dot_v;
	float x2 = x * x;
	float x5 = x2 * x2 * x;

	return f0 + (f90 - f0) * x5;
}

float diffuse_lambertian(float n_dot_l)
{
	return n_dot_l / k_pi;
}

float specular_ndf_ggx_tr(float n_dot_h, float linear_roughness)
{
	float roughness2 = linear_roughness * linear_roughness;

	float num = roughness2;
	float den = k_pi * pow((n_dot_h * n_dot_h) * (roughness2 - 1.0f) + 1.0f, 2);

	return num / den;
}

float specular_geometry_schlick_ggx(float n_dot_v, float linear_roughness)
{
	float k = pow(linear_roughness + 1.0f, 2) / 8.0f;

	return n_dot_v / (n_dot_v * (1.0f - k) + k);
}

float specular_shadowmask_smith_ggx(float n_dot_v, float n_dot_l, float linear_roughness)
{
	float ggx1 = specular_geometry_schlick_ggx(n_dot_v, linear_roughness);
	float ggx2 = specular_geometry_schlick_ggx(n_dot_l, linear_roughness);

	return ggx1 + ggx2;
}

float specular_ggx(float n_dot_v, float n_dot_l, float n_dot_h, float metal, float linear_roughness)
{
	float N = specular_ndf_ggx_tr(n_dot_h, linear_roughness);
	float D = specular_shadowmask_smith_ggx(n_dot_v, n_dot_l, linear_roughness);
	// TODO: This is the default f0 for plastic.  We need to figure out what to do with this value.
	float F = fresnel(float3(0.05f, 0.05f, 0.05f), 1.0f, n_dot_v);

	return (N * D * F) / k_pi;
}

float light_falloff(float power, float distance)
{
	return (power / (4 * k_pi)) / (distance * distance);
}

// Area light calculations adopted from https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
float get_sphere_illuminance(float power, float3 to_light, float dist, float n_dot_l, float radius)
{
	// Power correction.
	float luminance = power / (4.0f * (radius * radius) * (k_pi * k_pi));

	float radius2 = radius * radius;
	float illuminance = k_pi * (radius2 / (max(radius2, dist * dist))) * n_dot_l;

#if 0 // Horizon correction.
	float beta = acos(n_dot_l);
	float h = dist / radius;
	float x = sqrt(h * h - 1.0f);
	float y = -x * (1.0f / tan(beta));

	float illuminance = 0;
	if (h * cos(beta) > 1.0f)
	{
		illuminance = cos(beta) / (h * h);
	}
	else
	{
		illuminance = (1.0f / (k_pi * h * h)) *
			(cos(beta) * acos(y) - x * sin(beta) * sqrt(1 - y * y)) +
			(1.0f / k_pi) * atan(sin(beta) * sqrt(1 - y * y) / x);
	}
#endif

	return luminance * illuminance * k_pi;
}
