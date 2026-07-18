#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

class st_vk_command_allocator : public st_command_allocator
{
public:

	st_vk_command_allocator(
		const struct st_command_allocator_desc& desc,
		vk::Device* device,
		uint32_t queue_family_index);
	~st_vk_command_allocator();

	void reset() override;

	vk::CommandPool* get() { return &_command_pool; }

private:

	vk::CommandPool _command_pool;
	vk::Device* _device;
};

#endif