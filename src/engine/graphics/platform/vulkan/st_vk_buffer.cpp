/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_buffer.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

st_vk_buffer::st_vk_buffer(const uint32_t count, const size_t element_size, const e_st_buffer_usage_flags usage)
	: _count(count), _usage(usage), _element_size(element_size)
{
	st_vk_render_context::get()->create_buffer(_count * _element_size, _usage, _buffer, _memory);
}

st_vk_buffer::~st_vk_buffer()
{
	st_vk_render_context::get()->destroy_buffer(_buffer, _memory);
}

void st_vk_buffer::update(const class st_vk_render_context* context, void* data, const uint32_t count)
{
	st_vk_render_context::get()->update_buffer(_buffer, 0, count * _element_size, data);
}

#endif
