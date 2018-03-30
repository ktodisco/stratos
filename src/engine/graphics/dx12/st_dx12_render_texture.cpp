/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_render_texture.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <core/st_core.h>
#include <graphics/dx12/st_dx12_render_context.h>

st_dx12_render_texture::st_dx12_render_texture(
	uint32_t width,
	uint32_t height,
	e_st_texture_format format,
	const st_vec4f& clear)
{
	_format = format;

	st_dx12_render_context::get()->create_target(
		width,
		height,
		format,
		clear,
		_handle.GetAddressOf(),
		&_rtv,
		&_sampler,
		&_srv);
}

st_dx12_render_texture::~st_dx12_render_texture()
{
}

void st_dx12_render_texture::set_name(std::string name)
{
	ST_NAME_DX12_OBJECT(_handle.Get(), str_to_wstr(name).c_str());
}

#endif
