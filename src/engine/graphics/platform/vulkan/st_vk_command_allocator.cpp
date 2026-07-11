/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_command_allocator.h>

#if defined(ST_GRAPHICS_API_VULKAN)

st_vk_command_allocator::st_vk_command_allocator(vk::CommandPool* command_pool, vk::Device* device)
	: _command_pool(command_pool), _device(device)
{
}

st_vk_command_allocator::~st_vk_command_allocator()
{
	_command_pool = nullptr;
	_device = nullptr;
}

void st_vk_command_allocator::reset()
{
	VK_VALIDATE(_device->resetCommandPool(*_command_pool, vk::CommandPoolResetFlags(0)));
}

#endif
