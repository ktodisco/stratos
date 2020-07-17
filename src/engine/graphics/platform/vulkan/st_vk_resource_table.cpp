/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_resource_table.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

st_vk_resource_table::st_vk_resource_table()
{
	st_vk_render_context* context = st_vk_render_context::get();

	context->create_descriptor_set(st_descriptor_slot_textures, &_textures);
	context->create_descriptor_set(st_descriptor_slot_buffers, &_buffers);
	context->create_descriptor_set(st_descriptor_slot_constants, &_constants);
	context->create_descriptor_set(st_descriptor_slot_samplers, &_samplers);
}

st_vk_resource_table::~st_vk_resource_table()
{
	st_vk_render_context* context = st_vk_render_context::get();

	context->destroy_descriptor_set(_textures);
	context->destroy_descriptor_set(_buffers);
	context->destroy_descriptor_set(_constants);
	context->destroy_descriptor_set(_samplers);
}

#endif
