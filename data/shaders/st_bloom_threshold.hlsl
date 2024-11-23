struct vs_input
{
	float3 position : POSITION;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

[[vk::binding(0, 0)]] Texture2D tex : register(t0);
[[vk::binding(0, 1)]] SamplerState tex_sampler : register(s0);

[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
	float cutoff;
	
	float pad0;
	float pad1;
	float pad2;
}

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = float4(input.position.xy, 0.0f, 1.0f);
	result.uv = input.uv;

	return result;
};

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 source = tex.Sample(tex_sampler, input.uv).rgb;
	
	if (any(isnan(source)))
		source = 0.0f.xxx;
	
	float luma = dot(source, float3(0.2126f, 0.7152f, 0.0722f));
	float3 thresholded = luma > cutoff ? source : float3(0.0f, 0.0f, 0.0f);
	
	return float4(thresholded, 1.0f);
};
