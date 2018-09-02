#version 400

uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform vec4 u_eye;

in vec3 ps_in_position;
in vec3 ps_in_normal;
in vec2 ps_in_uv;
in vec3 ps_in_binormal;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_third;

void main(void)
{
	// Use the binormal and normal to find the tangent.
	vec3 tangent = normalize(cross(ps_in_normal, ps_in_binormal));
	
	// Construct the tangent space matrix.
	mat3 tangent_to_world;
	tangent_to_world[0] = ps_in_binormal;
	tangent_to_world[1] = tangent;
	tangent_to_world[2] = ps_in_normal;
	
	mat3 world_to_tangent = transpose(tangent_to_world);
	
	// Get the vector from the eye to the shading point.
	vec3 orig_from_eye = ps_in_position - u_eye.xyz;
	vec3 from_eye = world_to_tangent * orig_from_eye;
	
	float height_scale = 1.0;
	float parallax_limit = -length(from_eye.xy) / from_eye.z;
	
	vec2 offset_dir = normalize(from_eye.xy);
	vec2 max_offset = offset_dir * parallax_limit;
	
	int num_samples = int(mix(80, 40, dot(-normalize(orig_from_eye), ps_in_normal)));
	
	float step_size = 1.0 / float(num_samples);
	
	vec2 dx = dFdx(ps_in_uv);
	vec2 dy = dFdy(ps_in_uv);
	
	float current_ray_height = 1.0;
	vec2 current_offset = vec2(0.0, 0.0);
	vec2 last_offset = vec2(0.0, 0.0);
	float last_sampled_height = 1.0;
	float current_sampled_height = 1.0;
	int current_sample = 0;
	
	while (current_sample < num_samples)
	{
		current_sampled_height = textureGrad(u_normal, ps_in_uv + current_offset, dx, dy).a;
		if (current_sampled_height > current_ray_height)
		{
			float delta1 = current_sampled_height - current_ray_height;
			float delta2 = (current_ray_height + step_size) - last_sampled_height;
			float ratio = delta1 / (delta1 + delta2);
			current_offset = ratio * last_offset + (1.0 - ratio) * current_offset;
			current_sample = num_samples + 1;
		}
		else
		{
			current_sample++;
			current_ray_height -= step_size;
			last_offset = current_offset;
			current_offset += step_size * max_offset;
			last_sampled_height = current_sampled_height;
		}
	}
	
	vec2 final_coords = ps_in_uv + current_offset;
	vec3 final_normal = normalize(texture(u_normal, final_coords).xyz * world_to_tangent);
	vec3 final_color = texture(u_albedo, final_coords).xyz;
	
	out_albedo = vec4(final_color, 0.0);
	out_normal = vec4(normalize(ps_in_normal + final_normal) * 0.5 + 0.5, 0.0);
	out_third = vec4(0.0, 0.0, 0.0, 0.0);
}