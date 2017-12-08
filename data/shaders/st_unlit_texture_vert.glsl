#version 400

uniform mat4 u_mvp;

layout(location = 0) in vec3 in_vertex;
layout(location = 2) in vec2 in_texcood0;

out vec2 texcoord0;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;
	texcoord0 = in_texcood0;
}