#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

class st_gl_command_queue : public st_command_queue
{
public:

	st_gl_command_queue(HDC context);
	~st_gl_command_queue();

	void signal(struct st_fence* fence, uint64_t value) override;
	void wait(struct st_fence* fence, uint64_t value) override;
	void execute(class st_command_list* command_list) override;
	void present(struct st_swap_chain* swap_chain, uint32_t index) override;

private:

	HDC _device_context;
};

#endif
