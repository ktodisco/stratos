[[vk::binding(0, 2)]] cbuffer cb0 : register(b0)
{
    column_major float4x4 ProjectionMatrix;
};

struct VS_INPUT
{
    float2 pos : POSITION;
    float2 uv  : UV;
    float4 col : COLOR0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(float4(input.pos.xy, 0.f, 1.f), ProjectionMatrix);
    output.col = input.col;
    output.uv  = input.uv;
    return output;
}

[[vk::binding(0, 1)]] SamplerState sampler0 : register(s0);
[[vk::binding(0, 0)]] Texture2D texture0 : register(t0);

float4 ps_main(PS_INPUT input) : SV_Target
{
    float4 out_col = input.col * texture0.Sample(sampler0, input.uv);
    return out_col;
}