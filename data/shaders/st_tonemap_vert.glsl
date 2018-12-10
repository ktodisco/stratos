#version 400

layout(location = 0) in vec3 vs_in_position;

out vec2 ps_in_texcoord;

void main(void)
{
	vec2 texcoord_base = vec2(0.5, 0.5);
	ps_in_texcoord = vs_in_position.xy * texcoord_base + texcoord_base;
	
	gl_Position = vec4(vs_in_position.xy, 0.0, 1.0);
}
