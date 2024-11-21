// Octahedral encoding, from https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
float2 oct_wrap(float2 v)
{
    return (1.0f - abs(v.yx)) * (v.xy >= 0.0f ? 1.0f : -1.0f);   
}

float2 oct_encode(float3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    n.xy = (n.z >= 0.0f) ? n.xy : oct_wrap(n.xy);
    n.xy = n.xy * 0.5f + 0.5f;
    return n.xy;
}

float3 oct_decode(float2 f)
{
    f = f * 2.0f - 1.0f;
    
    float3 n = float3(f.x, f.y, 1.0f - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += (n.xy >= 0.0f) ? -t : t;
    return normalize(n);
}