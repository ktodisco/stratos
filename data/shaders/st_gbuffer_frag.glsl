#version 400

uniform sampler2D u_texture;

in vec3 ps_in_position;
in vec3 ps_in_normal;
in vec2 ps_in_uv;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_normal;

void main(void)
{
	out_albedo = texture(u_texture, ps_in_uv);
	out_normal = vec4(ps_in_normal, 1.0);
}