#version 400

uniform sampler2D u_texture;

in vec2 ps_in_texcoord;

layout(location = 0) out vec4 out_color;

vec3 aces_film(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
	vec3 result = clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);

	return result;
}

void main(void)
{
	out_color = vec4(aces_film(texture(u_texture, ps_in_texcoord).rgb), 1.0);
}
