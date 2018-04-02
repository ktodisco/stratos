#version 400

// Light constants.
const vec3 ambient = vec3(0.2, 0.2, 0.2);

uniform mat4 u_inverse_vp;
uniform vec4 u_light_position;
uniform vec4 u_light_color;
uniform float u_light_power;

uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_depth;

in vec2 ps_in_texcoord;

layout(location = 0) out vec4 out_color;

void main(void)
{
	vec3 albedo = texelFetch(u_albedo, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 normal = texelFetch(u_normal, ivec2(gl_FragCoord.xy), 0).rgb;
	float depth = texelFetch(u_depth, ivec2(gl_FragCoord.xy), 0).r;
	
	vec4 clip_position = vec4(ps_in_texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	vec4 world_position = clip_position * u_inverse_vp;
	world_position /= world_position.w;
	
	vec3 to_light = normalize(u_light_position.xyz - world_position.xyz);
	
	vec3 diffuse_color = max(dot(to_light, normal), 0) * albedo * u_light_color.rgb;
	vec3 lit_color = diffuse_color + (ambient * albedo);

	out_color = vec4(lit_color, 1.0);
}
