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

void load_shader(std::string filename, ID3DBlob** blob)
{
	std::wstring full_path =
		str_to_wstr(g_root_path) +
		str_to_wstr(filename) +
		str_to_wstr(".cso");

	D3DReadFileToBlob(full_path.c_str(), blob);
}

st_dx12_shader::st_dx12_shader(const char* filename, uint8_t type)
{
	if (type & st_shader_type_vertex)
	{
		load_shader(std::string(filename) + std::string("_vert"), _vs.GetAddressOf());
	}

	if (type & st_shader_type_pixel)
	{
		load_shader(std::string(filename) + std::string("_frag"), _ps.GetAddressOf());
	}

	// TODO: Other shader types.

	_type = type;
}

st_dx12_shader::~st_dx12_shader()
{
}

#endif
