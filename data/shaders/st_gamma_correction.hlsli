// Functions to handle gamma/linear space conversions.
// See https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch24.html

float3 gamma_to_linear(float3 x)
{
	return pow(x, 1.0f / 2.2f);
}

float3 linear_to_gamma(float3 x)
{
	return pow(x, 2.2f);
}
