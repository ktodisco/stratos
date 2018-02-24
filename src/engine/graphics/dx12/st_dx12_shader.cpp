/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_shader.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cassert>

#include <d3dcompiler.h>

extern char g_root_path[256];

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

st_dx12_shader::st_dx12_shader(const char* source, uint8_t type)
{
	// Compile the shaders from source.
	uint32_t compile_flags = 0;
#if defined(_DEBUG)
	compile_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	std::wstring full_path = s2ws(g_root_path) + s2ws(source) + s2ws(".hlsl");

	if (type & st_shader_type_vertex)
	{
		HRESULT result = D3DCompileFromFile(
			full_path.c_str(),
			nullptr,
			nullptr,
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
			nullptr,
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
