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

st_dx12_texture::st_dx12_texture()
{
}

st_dx12_texture::st_dx12_texture(uint32_t width, uint32_t height) :
	_width(width), _height(height)
{
}

st_dx12_texture::~st_dx12_texture()
{
}

void st_dx12_texture::load_from_data(uint32_t width, uint32_t height, uint32_t levels, e_st_format format, void* data)
{
	_format = format;
	_levels = levels;

	st_dx12_render_context::get()->create_texture(
		width,
		height,
		levels,
		format,
		data,
		_handle.GetAddressOf(),
		&_sampler,
		&_srv);
}

void st_dx12_texture::bind(class st_dx12_render_context* context)
{
	context->set_shader_resource_table(_srv);
	context->set_sampler_table(_sampler);
}

void st_dx12_texture::set_name(std::string name)
{
	ST_NAME_DX12_OBJECT(_handle.Get(), str_to_wstr(name).c_str());
}

#endif
