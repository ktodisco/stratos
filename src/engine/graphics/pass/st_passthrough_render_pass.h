#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>

class st_passthrough_render_pass : public st_fullscreen_render_pass
{
public:
	st_passthrough_render_pass(
		class st_render_texture* source_buffer,
		struct st_swap_chain* swap_chain);
	~st_passthrough_render_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<struct st_render_pass> _pass = nullptr;
	std::unique_ptr<struct st_framebuffer> _framebuffers[k_max_frames];
};
