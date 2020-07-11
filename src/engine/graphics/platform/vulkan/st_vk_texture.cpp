/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_texture.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

st_vk_texture::st_vk_texture()
{

}

st_vk_texture::st_vk_texture(uint32_t width, uint32_t height)
	: _width(width), _height(height)
{

}

st_vk_texture::~st_vk_texture()
{

}

void st_vk_texture::load_from_data(
	uint32_t width,
	uint32_t height,
	uint32_t levels,
	e_st_format format,
	void* data)
{
	_format = format;
	_levels = levels;

	st_vk_render_context::get()->create_texture(
		width,
		height,
		levels,
		format,
		data,
		_handle);
}

void st_vk_texture::bind(class st_dx12_render_context* context)
{

}

void st_vk_texture::set_name(std::string name)
{

}

#endif
