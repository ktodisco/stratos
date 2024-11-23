#include "st_gamma_correction.hlsli"

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
};

[[vk::binding(0, 2)]]
cbuffer cb0 : register(b0)
{
    column_major float4x4 mvp;
}

ps_input vs_main(vs_input input)
{
    ps_input result;

    result.position = mul(float4(input.position, 1.0f), mvp);

    return result;
}

float ps_main(ps_input input) : SV_Depth
{
    return input.position.z;
}
