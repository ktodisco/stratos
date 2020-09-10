/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_texture.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

#include <graphics/st_render_context.h>

st_vk_texture::st_vk_texture(
	uint32_t width,
	uint32_t height,
	uint32_t levels,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	void* data)
	: _width(width), _height(height), _levels(levels), _format(format), _usage(usage)
{
	st_vk_render_context* context = st_vk_render_context::get();

	context->create_texture(
		width,
		height,
		levels,
		format,
		usage,
		initial_state,
		data,
		_handle,
		_memory);
	st_vk_render_context::get()->create_texture_view(this, _view);

	_state = initial_state;
}

st_vk_texture::~st_vk_texture()
{
	st_vk_render_context::get()->destroy_texture_view(_view);
	st_vk_render_context::get()->destroy_texture(_handle, _memory);
}

void st_vk_texture::bind(class st_dx12_render_context* context)
{

}

void st_vk_texture::set_name(std::string name)
{

}

void st_vk_texture::transition(st_vk_render_context* context, e_st_texture_state new_state)
{
	if (new_state != _state)
	{
		context->transition(this, _state, new_state);
		_state = new_state;
	}
}

#endif
