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

	st_vk_command_allocator(vk::CommandPool* command_allocator, vk::Device* device);
	~st_vk_command_allocator();

	void reset() override;

private:

	vk::CommandPool* _command_pool;
	vk::Device* _device;
};

#endif