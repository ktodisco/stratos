#version 400

layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec3 vs_in_normal;
layout(location = 3) in vec2 vs_in_uv;

uniform mat4 u_mvp;

out vec3 ps_in_position;
out vec3 ps_in_normal;
out vec2 ps_in_uv;

void main(void)
{
	ps_in_position = vs_in_position;
	ps_in_normal = vs_in_normal;
	ps_in_uv = vs_in_uv;
	gl_Position = vec4(vs_in_position, 1.0) * u_mvp;
}