#version 400

uniform sampler2D u_albedo;
uniform sampler2D u_mge;
uniform float u_emissive;

in vec3 ps_in_position;
in vec3 ps_in_normal;
in vec2 ps_in_uv;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_third;

void main(void)
{
	vec4 mge = texture(u_mge, ps_in_uv);
	
	out_albedo = vec4(texture(u_albedo, ps_in_uv).rgb, mge.r);
	out_normal = vec4(normalize(ps_in_normal) * 0.5 + 0.5, mge.g);
	out_third = vec4(mge.b * u_emissive, 0.0, 0.0, 0.0);
}