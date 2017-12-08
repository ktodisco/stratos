#version 400

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

uniform mat4 u_mvp;

out vec3 o_position;
out vec3 o_normal;

void main(void)
{
	o_position = in_position;
	o_normal = in_normal;
	gl_Position = vec4(in_position, 1.0) * u_mvp;
}