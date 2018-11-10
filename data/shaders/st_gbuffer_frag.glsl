#version 400

uniform sampler2D u_albedo;
uniform sampler2D u_mre;
uniform float u_emissive;

in vec3 ps_in_position;
in vec3 ps_in_normal;
in vec2 ps_in_uv;
in vec3 ps_in_tangent;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_third;

void main(void)
{
	vec4 mre = texture(u_mre, ps_in_uv);
	
	out_albedo = vec4(texture(u_albedo, ps_in_uv).rgb, mre.r);
	out_normal = vec4(normalize(ps_in_normal) * 0.5 + 0.5, mre.g);
	out_third = vec4(mre.b * u_emissive, 0.0, 0.0, 0.0);
}