#version 400

uniform vec3 u_color;
uniform sampler2D u_texture;

in vec2 texcoord0;

void main(void)
{
	gl_FragColor = vec4(u_color, texture(u_texture, texcoord0).x);
}