struct vs_input
{
	float3 position : POSITION;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

[[vk::binding(0, 0)]] Texture2D tex : register(t0);
[[vk::binding(0, 1)]] SamplerState tex_sampler : register(s0);

ps_input vs_main(vs_input input)
{
	ps_input result;
	
	float2 texcoord_base = float2(0.5f, 0.5f);
	result.uv = input.position.xy * texcoord_base + texcoord_base;
	result.uv.y = 1.0f - result.uv.y;
	
	result.position = float4(input.position.xy, 0.0f, 1.0f);
	
	return result;
};

float3 aces_film(float3 x)
{
	// Tonemap down to the reference white level of 80 nits.
	const float k_ref_white = 80.0f;
	x /= k_ref_white;
	
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
	float3 result = saturate((x * (a * x + b)) / (x * (c * x + d) + e));

	return result;
}

float3 gamma_correct(float3 x)
{
	float3 low = x * 12.92f;
	float3 high = (pow(abs(x), 1.0f / 2.4f) * 1.055f) - 0.055f;
	float3 srgb = (x <= 0.0031308f) ? low : high;
	return srgb;
}

float4 ps_main(ps_input input) : SV_TARGET
{
	float3 tonemap = aces_film(tex.Sample(tex_sampler, input.uv).rgb);
	float3 gamma = gamma_correct(tonemap);
	
	return float4(gamma, 1.0f);
};