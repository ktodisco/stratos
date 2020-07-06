/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_constant_buffer.h>

#include <graphics/platform/vulkan/st_vk_render_context.h>

st_vk_constant_buffer::st_vk_constant_buffer(const size_t size)
	: _size(size)
{
	st_vk_render_context::get()->create_buffer(_size, st_uniform_buffer, _buffer);
}

st_vk_constant_buffer::~st_vk_constant_buffer()
{
	st_vk_render_context::get()->destroy_buffer(_buffer);
}

void st_vk_constant_buffer::update(const class st_vk_render_context* context, void* data)
{
	st_vk_render_context::get()->update_buffer(_buffer, 0, _size, data);
}
