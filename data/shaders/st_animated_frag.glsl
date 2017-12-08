#version 400

in vec3 o_normal;
in vec3 o_color;

void main(void)
{
	gl_FragColor = vec4(o_color, 1.0);
}
