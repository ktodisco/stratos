#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <memory>

/*
** A render pass that draws the static scene objects to the gbuffer.
*/
class st_directional_shadow_pass
{
public:
	st_directional_shadow_pass(class st_render_texture* target);
	~st_directional_shadow_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

	void get_target_formats(struct st_graphics_state_desc& desc);

private:
	class st_render_texture* _target;

	std::unique_ptr<struct st_render_pass> _pass = nullptr;
	std::unique_ptr<struct st_framebuffer> _framebuffer = nullptr;
};
