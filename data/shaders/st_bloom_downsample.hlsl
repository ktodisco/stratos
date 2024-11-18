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
	float4 target_dim;
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
	// Sample algorithm from Call of Duty's Next Gen Postprocessing presentation at SIGGRAPH 2014.
	float3 tls = tex.Sample(tex_sampler, input.uv + float2(-1.0f, -1.0f) * target_dim.zw).rgb;
	float3 tts = tex.Sample(tex_sampler, input.uv + float2( 0.0f, -1.0f) * target_dim.zw).rgb;
	float3 trs = tex.Sample(tex_sampler, input.uv + float2( 1.0f, -1.0f) * target_dim.zw).rgb;
	float3 lls = tex.Sample(tex_sampler, input.uv + float2(-1.0f,  0.0f) * target_dim.zw).rgb;
	float3 ccs = tex.Sample(tex_sampler, input.uv + float2( 0.0f,  0.0f) * target_dim.zw).rgb;
	float3 rrs = tex.Sample(tex_sampler, input.uv + float2( 1.0f,  0.0f) * target_dim.zw).rgb;
	float3 bls = tex.Sample(tex_sampler, input.uv + float2(-1.0f,  1.0f) * target_dim.zw).rgb;
	float3 bbs = tex.Sample(tex_sampler, input.uv + float2( 0.0f,  1.0f) * target_dim.zw).rgb;
	float3 brs = tex.Sample(tex_sampler, input.uv + float2( 1.0f,  1.0f) * target_dim.zw).rgb;
	float3 m0s = tex.Sample(tex_sampler, input.uv + float2(-0.5f, -0.5f) * target_dim.zw).rgb;
	float3 m1s = tex.Sample(tex_sampler, input.uv + float2( 0.5f, -0.5f) * target_dim.zw).rgb;
	float3 m2s = tex.Sample(tex_sampler, input.uv + float2(-0.5f,  0.5f) * target_dim.zw).rgb;
	float3 m3s = tex.Sample(tex_sampler, input.uv + float2( 0.5f,  0.5f) * target_dim.zw).rgb;
	
	float3 tl = (tls + tts + lls + ccs) * 0.25f;
	float3 tr = (tts + trs + ccs + rrs) * 0.25f;
	float3 bl = (lls + ccs + bls + bbs) * 0.25f;
	float3 br = (ccs + rrs + bbs + brs) * 0.25f;
	float3 ma = (m0s + m1s + m2s + m3s) * 0.25f;
	
	float3 filtered = (ma * 0.5f) + ((tl + tr + bl + br) * 0.125f);
	
	// Simulate a physical reduction in the radiance that spreads out on the photo receptor.
	return float4(filtered * 0.45f, 1.0f);
};
