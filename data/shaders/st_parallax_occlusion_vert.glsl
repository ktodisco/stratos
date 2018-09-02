#version 400

layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec3 vs_in_normal;
layout(location = 2) in vec4 vs_in_color;
layout(location = 3) in vec2 vs_in_uv;
layout(location = 4) in vec3 vs_in_binormal;

uniform mat4 u_model;
uniform mat4 u_mvp;

out vec3 ps_in_position;
out vec3 ps_in_normal;
out vec2 ps_in_uv;
out vec3 ps_in_binormal;

void main(void)
{
	ps_in_position = (vec4(vs_in_position, 1.0) * u_model).xyz;
	ps_in_normal = normalize(vs_in_normal * mat3(u_model));
	ps_in_uv = vs_in_uv;
	ps_in_binormal = normalize(vs_in_binormal * mat3(u_model));
	gl_Position = vec4(vs_in_position, 1.0) * u_mvp;
}
