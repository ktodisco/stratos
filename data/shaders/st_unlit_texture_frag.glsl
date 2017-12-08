#version 400

uniform sampler2D u_texture;

in vec2 texcoord0;

void main(void)
{
	gl_FragColor = texture(u_texture, texcoord0);
}