#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <memory>

class st_deferred_light_render_pass : public st_fullscreen_render_pass
{
public:
	st_deferred_light_render_pass(
		class st_render_texture* albedo_buffer,
		class st_render_texture* normal_buffer,
		class st_render_texture* depth_buffer,
		class st_render_texture* output_buffer,
		class st_render_texture* output_depth);
	~st_deferred_light_render_pass();

	void render(class st_render_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<class st_framebuffer> _framebuffer = nullptr;

	std::unique_ptr<class st_point_light> _light = nullptr;
	std::unique_ptr<class st_constant_buffer> _light_buffer = nullptr;
};