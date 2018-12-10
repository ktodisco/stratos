#version 400

// Light constants.
const float s_pi = 3.14159;
const float ambient = 10.0;

uniform mat4 u_inverse_vp;
uniform vec4 u_eye;
uniform vec4 u_light_position;
uniform vec4 u_light_color;
uniform float u_light_power;

uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_third;
uniform sampler2D u_depth;

in vec2 ps_in_texcoord;

layout(location = 0) out vec4 out_color;

float light_falloff(float power, float distance)
{
	return (power / (4.0 * s_pi)) / (distance * distance);
}

vec3 diffuse_lambertian(vec3 albedo, vec3 n, vec3 l)
{
	return (albedo / s_pi) * clamp(dot(n, l), 0.0, 1.0);
}

vec3 specular_phong(vec3 specular, vec3 n, vec3 v, vec3 l, float roughness)
{
	vec3 r = reflect(-l, n);
	float a = clamp(dot(r, v), 0.0, 1.0);
	return specular * pow(a, (roughness * 50.0f) / 4.0f);
}

void main(void)
{
	vec4 albedo_sample = texelFetch(u_albedo, ivec2(gl_FragCoord.xy), 0);
	vec4 normal_sample = texelFetch(u_normal, ivec2(gl_FragCoord.xy), 0);
	vec4 third_sample = texelFetch(u_third, ivec2(gl_FragCoord.xy), 0);
	float depth = texelFetch(u_depth, ivec2(gl_FragCoord.xy), 0).r;
	
	vec3 albedo = albedo_sample.rgb;
	float metalness = albedo_sample.a;
	vec3 normal = normal_sample.rgb * 2.0 - 1.0;
	float roughness = normal_sample.a;
	float emissive = third_sample.r;
	
	vec4 clip_position = vec4(ps_in_texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	vec4 world_position = clip_position * u_inverse_vp;
	world_position /= world_position.w;
	
	vec3 to_eye = normalize(u_eye.xyz - world_position.xyz);
	vec3 to_light = normalize(vec3(-1.0, 1.0, -1.0));//u_light_position.xyz - world_position.xyz;
	float dist_to_light = length(to_light);
	to_light = normalize(to_light);
	
	vec3 diffuse_color = albedo * (1.0 - metalness);
	vec3 specular_color = u_light_color.rgb * metalness;
	
	vec3 lit_color = diffuse_lambertian(diffuse_color, normal, to_light) * light_falloff(u_light_power, dist_to_light);
	lit_color += specular_phong(specular_color, normal, to_eye, to_light, roughness) * light_falloff(u_light_power, dist_to_light);
	lit_color += diffuse_color * ambient;
	
	lit_color += emissive * albedo_sample.rgb;

	out_color = vec4(lit_color, 1.0);
}
