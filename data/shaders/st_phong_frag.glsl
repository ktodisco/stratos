#version 400

// Light constants.
const vec3 light_dir = normalize(vec3(-1.0, -1.0, -1.0));
const vec3 ambient = vec3(0.2, 0.2, 0.2);
const vec3 diffuse = vec3(0.5, 0.5, 0.5);
const vec3 specular = vec3(0.5, 0.5, 0.5);

in vec3 o_position;
in vec3 o_normal;

uniform vec3 u_eye;

void main(void)
{
	vec3 to_light = -light_dir;
	
	vec3 diffuse_color = diffuse * max(dot(to_light, o_normal), 0);
	vec3 lit_color = ambient + diffuse_color;
	
	gl_FragColor = vec4(lit_color, 1.0);
}