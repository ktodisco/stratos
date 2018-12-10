#version 400

uniform sampler2D u_texture;

in vec2 ps_in_texcoord;

layout(location = 0) out vec4 out_color;

vec3 aces_film(vec3 x)
{
	// Tonemap down to the reference white level of 80 nits.
	const float k_ref_white = 80.0;
	x /= k_ref_white;
	
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
	vec3 result = clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);

	return result;
}

vec3 gamma_correct(vec3 x)
{
	return pow(x, vec3(2.2));
}

void main(void)
{
	vec3 tonemap = aces_film(texture(u_texture, ps_in_texcoord).rgb);
	vec3 gamma = gamma_correct(tonemap);
	out_color = vec4(gamma, 1.0);
}
