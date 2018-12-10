#version 400

in vec3 o_normal;
in vec2 o_texcoord;

uniform sampler2D u_texture;

void main(void)
{
	gl_FragColor = texture(u_texture, o_texcoord);
}
