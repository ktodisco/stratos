// Functions to handle gamma/linear space conversions.
// See https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch24.html

float3 approx_srgb_to_linear(float3 x)
{
	return pow(x, 1.0f / 2.2f);
}

float3 approx_linear_to_srgb(float3 x)
{
	return pow(x, 2.2f);
}

float3 srgb_to_linear(float3 x)
{
	float3 low = x / 12.92f;
	float3 high = pow((x + 0.055f) / 1.055f, 2.4f);
	float3 l = select(x <= 0.04045f, low, high);
	return l;
}

float3 linear_to_srgb(float3 x)
{
	float3 low = x * 12.92f;
	float3 high = (pow(abs(x), 1.0f / 2.4f) * 1.055f) - 0.055f;
	float3 srgb = select(x <= 0.0031308f, low, high);
	return srgb;
}
