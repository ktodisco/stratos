/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_command_queue.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_command_list.h>

#include <cassert>

st_vk_command_queue::st_vk_command_queue(vk::Queue* queue)
	: _queue(queue)
{
}

st_vk_command_queue::~st_vk_command_queue()
{
	_queue = nullptr;
}

void st_vk_command_queue::signal(st_fence* fence_, uint64_t value)
{
}

void st_vk_command_queue::wait(st_fence* fence_, uint64_t value)
{
}

void st_vk_command_queue::execute(st_command_list* command_list_)
{
}

void st_vk_command_queue::present(st_swap_chain* swap_chain_)
{
}

#endif
