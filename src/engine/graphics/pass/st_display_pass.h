#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>

class st_display_pass : public st_fullscreen_render_pass
{
public:

	st_display_pass(class st_render_texture* source, struct st_swap_chain* swap_chain);
	~st_display_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<struct st_buffer> _cb;
	std::unique_ptr<struct st_buffer_view> _cbv;
	std::unique_ptr<struct st_pipeline> _pipeline;
	std::unique_ptr<struct st_resource_table> _resources;
	std::unique_ptr<struct st_render_pass> _pass;
	std::unique_ptr<struct st_framebuffer> _framebuffers[k_max_frames];

	st_render_texture* _source = nullptr;
};