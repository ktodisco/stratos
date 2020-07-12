/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_texture.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

st_vk_texture::st_vk_texture(
	uint32_t width,
	uint32_t height,
	uint32_t levels,
	e_st_format format,
	e_st_texture_usage_flags usage)
	: _width(width), _height(height), _levels(levels), _format(format), _usage(usage)
{
	st_vk_render_context::get()->create_texture(
		width,
		height,
		levels,
		format,
		usage,
		_handle);
	st_vk_render_context::get()->create_texture_view(this, _view);
}

st_vk_texture::~st_vk_texture()
{

}

void st_vk_texture::load_from_data(void* data)
{
	st_vk_render_context::get()->upload_texture(this, data);
}

void st_vk_texture::bind(class st_dx12_render_context* context)
{

}

void st_vk_texture::set_name(std::string name)
{

}

#endif
