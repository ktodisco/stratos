#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_descriptor_heap.h>

#include <cstdint>
#include <vector>

class st_vk_resource_table
{
public:
	st_vk_resource_table();
	~st_vk_resource_table();

	void add_constant_buffer(class st_vk_constant_buffer* cb) {}
	void add_texture_resource(class st_vk_texture* sr) {}
	void add_buffer_resource(class st_vk_buffer* br) {}

	void bind(class st_vk_render_context* context) {}

private:
	vk::DescriptorSet _textures;
	vk::DescriptorSet _constants;
	vk::DescriptorSet _buffers;
	vk::DescriptorSet _samplers;
};

#endif
