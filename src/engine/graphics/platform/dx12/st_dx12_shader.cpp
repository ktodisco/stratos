/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_shader.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <core/st_core.h>

#include <cassert>

#include <d3dcompiler.h>

extern char g_root_path[256];

st_dx12_shader::st_dx12_shader(const char* source, uint8_t type)
{
	// Compile the shaders from source.
	uint32_t compile_flags = 0;
#if defined(_DEBUG)
	compile_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	std::wstring full_path =
		str_to_wstr(g_root_path) +
		str_to_wstr(source) +
		str_to_wstr(".hlsl");

	if (type & st_shader_type_vertex)
	{
		HRESULT result = D3DCompileFromFile(
			full_path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"vs_main",
			"vs_5_0",
			compile_flags,
			0,
			&_vs,
			&errors);

		if (result != S_OK)
		{
			if (errors)
			{
				OutputDebugStringA((char*)errors->GetBufferPointer());
			}

			assert(false);
		}
	}

	if (type & st_shader_type_pixel)
	{
		HRESULT result = D3DCompileFromFile(
			(LPCWSTR)full_path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"ps_main",
			"ps_5_0",
			compile_flags,
			0,
			&_ps,
			&errors);

		if (result != S_OK)
		{
			if (errors)
			{
				OutputDebugStringA((char*)errors->GetBufferPointer());
			}

			assert(false);
		}
	}

	// TODO: Other shader types.

	_type = type;
}

st_dx12_shader::~st_dx12_shader()
{
}

#endif
