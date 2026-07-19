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

	st_vk_command_queue(const st_command_queue_desc& desc, class st_vk_device* device, uint32_t queue_family_index);
	~st_vk_command_queue();

	void signal(struct st_fence* fence, uint64_t value) override;
	void wait(struct st_fence* fence, uint64_t value) override;
	void execute(class st_command_list* command_list) override;
	void present(struct st_swap_chain* swap_chain, uint32_t index) override;

private:

	vk::Queue _queue;
	class st_vk_device* _device;
};

#endif
