/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_command_allocator.h>

#if defined(ST_GRAPHICS_API_VULKAN)

st_vk_command_allocator::st_vk_command_allocator(
	const st_command_allocator_desc& desc,
	vk::Device* device,
	uint32_t queue_family_index)
	: _device(device)
{
	vk::CommandPoolCreateInfo command_pool_info = vk::CommandPoolCreateInfo()
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
		.setQueueFamilyIndex(queue_family_index);

	VK_VALIDATE(_device->createCommandPool(&command_pool_info, nullptr, &_command_pool));
}

st_vk_command_allocator::~st_vk_command_allocator()
{
	_device->destroyCommandPool(_command_pool, nullptr);
	_device = nullptr;
}

void st_vk_command_allocator::reset()
{
	VK_VALIDATE(_device->resetCommandPool(_command_pool, vk::CommandPoolResetFlags(0)));
}

#endif
