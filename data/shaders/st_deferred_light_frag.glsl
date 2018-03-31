#version 400

// Light constants.
const vec3 ambient = vec3(0.2, 0.2, 0.2);

uniform vec4 u_light_position;
uniform vec4 u_light_color;
uniform float u_light_power;

uniform sampler2D u_albedo;
uniform sampler2D u_normal;

in vec2 ps_in_texcoord;

layout(location = 0) out vec4 out_color;

void main(void)
{
	vec3 albedo = texelFetch(u_albedo, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 normal = texelFetch(u_normal, ivec2(gl_FragCoord.xy), 0).rgb;
	
	vec3 to_light = -normalize(u_light_position.xyz);
	
	vec3 diffuse_color = max(dot(to_light, normal), 0) * albedo * u_light_color.rgb;
	vec3 lit_color = diffuse_color + (ambient * albedo);

	out_color = vec4(lit_color, 1.0);
}
