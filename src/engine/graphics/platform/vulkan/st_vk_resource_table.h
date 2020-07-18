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

	void set_constant_buffers(uint32_t count, class st_constant_buffer** cbs);
	void set_textures(uint32_t count, class st_texture** textures);
	void set_buffers(uint32_t count, class st_buffer** buffers);

	void bind(class st_vk_render_context* context) {}

private:
	vk::DescriptorSet _textures;
	vk::DescriptorSet _constants;
	vk::DescriptorSet _buffers;
	vk::DescriptorSet _samplers;
};

#endif
