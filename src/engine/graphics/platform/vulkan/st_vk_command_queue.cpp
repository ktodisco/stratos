/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_command_queue.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_command_list.h>
#include <graphics/platform/vulkan/st_vk_device.h>

#include <cassert>

st_vk_command_queue::st_vk_command_queue(const st_command_queue_desc& desc, st_vk_device* device, uint32_t queue_family_index)
	: _device(device)
{
	_device->get()->getQueue(queue_family_index, 0, &_queue);
}

st_vk_command_queue::~st_vk_command_queue()
{
	VK_VALIDATE(_queue.waitIdle());
}

void st_vk_command_queue::signal(st_fence* fence_, uint64_t value)
{
	st_vk_fence* fence = static_cast<st_vk_fence*>(fence_);

	vk::TimelineSemaphoreSubmitInfo semaphore_info = vk::TimelineSemaphoreSubmitInfo()
		.setPSignalSemaphoreValues(&value)
		.setSignalSemaphoreValueCount(1);

	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setPNext(&semaphore_info)
		.setPSignalSemaphores(&fence->_semaphore)
		.setSignalSemaphoreCount(1);

	VK_VALIDATE(_queue.submit(1, &submit_info, VK_NULL_HANDLE));
}

void st_vk_command_queue::wait(st_fence* fence_, uint64_t value)
{
	st_vk_fence* fence = static_cast<st_vk_fence*>(fence_);

	vk::TimelineSemaphoreSubmitInfo semaphore_info = vk::TimelineSemaphoreSubmitInfo()
		.setPWaitSemaphoreValues(&value)
		.setWaitSemaphoreValueCount(1);

	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setPNext(&semaphore_info)
		.setPWaitSemaphores(&fence->_semaphore)
		.setWaitSemaphoreCount(1);

	VK_VALIDATE(_queue.submit(1, &submit_info, VK_NULL_HANDLE));
}

void st_vk_command_queue::execute(st_command_list* command_list_)
{
	st_vk_command_list* command_list = static_cast<st_vk_command_list*>(command_list_);

	vk::CommandBuffer* buffer = command_list->get();

	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(buffer);

	VK_VALIDATE(_queue.submit(1, &submit_info, VK_NULL_HANDLE));
}

void st_vk_command_queue::present(st_swap_chain* swap_chain_, uint32_t index)
{
	st_vk_swap_chain* swap_chain = static_cast<st_vk_swap_chain*>(swap_chain_);

	vk::PresentInfoKHR present_info = vk::PresentInfoKHR()
		.setSwapchainCount(1)
		.setPSwapchains(&swap_chain->_swap_chain)
		.setPImageIndices(&index);

	VK_VALIDATE(_queue.presentKHR(&present_info));
}

#endif
