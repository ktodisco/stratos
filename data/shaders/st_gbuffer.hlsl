struct vs_input
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : UV;
};

struct ps_input
{
	float4 position : SV_POSITION;
	float3 normal : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

cbuffer cb0 : register(b0)
{
	float4x4 mvp;
}

Texture2D diffuse_texture : register(t0);
Texture2D mge_texture : register(t1);
SamplerState diffuse_sampler : register(s0);
SamplerState mge_sampler : register(s1);

ps_input vs_main(vs_input input)
{
	ps_input result;

	result.position = mul(float4(input.position, 1.0f), mvp);
	result.normal = input.normal;
	result.uv = input.uv;

	return result;
}

struct ps_output
{
	float4 albedo : SV_Target0;
	float4 normal : SV_Target1;
};

ps_output ps_main(ps_input input)
{
	ps_output output;
	
	float4 mge = mge_texture.Sample(mge_sampler, input.uv);
	
	output.albedo = float4(diffuse_texture.Sample(diffuse_sampler, input.uv).rgb, mge.r);
	output.normal = float4(normalize(input.normal) * 0.5f + 0.5f, mge.g);
	
	return output;
}
