#version 400

uniform mat4 u_mvp;

layout(location = 0) in vec3 in_vertex;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0) * u_mvp;
}