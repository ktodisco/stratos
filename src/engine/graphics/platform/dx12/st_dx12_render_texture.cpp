/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_render_texture.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_render_context.h>

st_dx12_render_texture::st_dx12_render_texture(
	uint32_t width,
	uint32_t height,
	e_st_texture_format format,
	const st_vec4f& clear) : st_texture(width, height)
{
	_format = format;

	st_dx12_render_context::get()->create_target(
		width,
		height,
		format,
		clear,
		_handle.GetAddressOf(),
		&_rtv);
}

st_dx12_render_texture::~st_dx12_render_texture()
{
	st_dx12_render_context::get()->destroy_target(_rtv);
}

#endif
