#version 400

uniform sampler2D u_texture;

in vec2 ps_in_texcoord;

layout(location = 0) out vec4 out_color;

void main(void)
{
	out_color = texture(u_texture, ps_in_texcoord);
}
