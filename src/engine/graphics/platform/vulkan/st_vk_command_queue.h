#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

class st_vk_command_queue : public st_command_queue
{
public:

	st_vk_command_queue(const st_command_queue_desc& desc, vk::Device* device, uint32_t queue_family_index);
	~st_vk_command_queue();

	void signal(struct st_fence* fence, uint64_t value) override;
	void wait(struct st_fence* fence, uint64_t value) override;
	void execute(class st_command_list* command_list) override;
	void present(struct st_swap_chain* swap_chain) override;

private:

	vk::Device* _device;
	vk::Queue _queue;

	uint32_t _frame_index = 0;
};

#endif
