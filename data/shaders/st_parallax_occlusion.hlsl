struct vs_input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 color : COLOR;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float3 normal : TEXCOORD0;
	float3 tangent : TEXCOORD1;
	float2 uv : TEXCOORD2;
};

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	float4x4 model;
	float4x4 mvp;
	float4 eye;
}

[[vk::binding(0, 0)]] Texture2D diffuse_texture : register(t0);
[[vk::binding(1, 0)]] Texture2D normal_texture : register(t1);
[[vk::binding(2, 0)]] Texture2D mre_texture : register(t2);
[[vk::binding(0, 1)]] SamplerState diffuse_sampler : register(s0);
[[vk::binding(1, 1)]] SamplerState normal_sampler : register(s1);
[[vk::binding(2, 1)]] SamplerState mre_sampler : register(s2);

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = mul(float4(input.position, 1.0f), mvp);
	result.normal = mul(float4(input.normal, 0.0f), model).xyz;
	result.tangent = mul(float4(input.position, 0.0f), model).xyz;
	result.uv = input.uv;

	return result;
}

struct ps_output
{
	float4 albedo : SV_Target0;
	float4 normal : SV_Target1;
	float4 third : SV_Target2;
};

ps_output ps_main(ps_input input)
{
	ps_output output;
	
	// Use the tangent and normal to find the binormal.
	float3 binormal = normalize(cross(input.normal, input.tangent));
	
	// Construct the tangent space matrix.
	float3x3 tangent_to_world;
	tangent_to_world[0] = input.tangent;
	tangent_to_world[1] = binormal;
	tangent_to_world[2] = input.normal;
	
	float3x3 world_to_tangent = transpose(tangent_to_world);
	
	// Get the vector from the eye to the shading point.
	float3 orig_from_eye = input.position.xyz - eye.xyz;
	float3 from_eye = mul(world_to_tangent, orig_from_eye);
	
	float height_scale = 1.0f;
	float parallax_limit = -length(from_eye.xy) / from_eye.z;
	
	float2 offset_dir = normalize(from_eye.xy);
	float2 max_offset = offset_dir * parallax_limit;
	
	int num_samples = int(lerp(80, 40, dot(-normalize(orig_from_eye), input.normal)));
	
	float step_size = 1.0f / float(num_samples);
	
	float2 dx = ddx(input.uv);
	float2 dy = ddy(input.uv);
	
	float current_ray_height = 1.0f;
	float2 current_offset = float2(0.0f, 0.0f);
	float2 last_offset = float2(0.0f, 0.0f);
	float last_sampled_height = 1.0f;
	float current_sampled_height = 1.0f;
	int current_sample = 0;
	
	/*while (current_sample < num_samples)
	{
		current_sampled_height = normal_texture.SampleGradient(normal_sampler, input.uv + current_offset, dx, dy).a;
		if (current_sampled_height > current_ray_height)
		{
			float delta1 = current_sampled_height - current_ray_height;
			float delta2 = (current_ray_height + step_size) - last_sampled_height;
			float ratio = delta1 / (delta1 + delta2);
			current_offset = ratio * last_offset + (1.0f - ratio) * current_offset;
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
	}*/
	
	float2 final_coords = input.uv + current_offset;
	float3 final_normal = normalize(mul(world_to_tangent, normal_texture.Sample(normal_sampler, final_coords).xyz));
	float3 final_color = diffuse_texture.Sample(diffuse_sampler, final_coords).xyz;
	
	output.albedo = float4(final_color, 0.5f);
	output.normal = float4(normalize(input.normal + final_normal) * 0.5f + 0.5f, 0.0);
	output.third = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	return output;
}