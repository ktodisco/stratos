/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_texture.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <core/st_core.h>

#include <graphics/platform/dx12/st_dx12_render_context.h>

#include <string>

st_dx12_texture::st_dx12_texture(
	uint32_t width,
	uint32_t height,
	uint32_t levels,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	void* data)
	: _width(width), _height(height), _levels(levels), _format(format), _usage(usage)
{
	st_dx12_render_context::get()->create_texture(
		width,
		height,
		levels,
		format,
		usage,
		initial_state,
		data,
		_handle.GetAddressOf());

	_state = initial_state;
}

st_dx12_texture::~st_dx12_texture()
{
	_handle = nullptr;
}

void st_dx12_texture::transition(st_dx12_render_context* context, e_st_texture_state new_state)
{
	context->transition(this, _state, new_state);
	_state = new_state;
}

void st_dx12_texture::set_name(std::string name)
{
	ST_NAME_DX12_OBJECT(_handle.Get(), str_to_wstr(name).c_str());
}

#endif
