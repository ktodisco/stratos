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
		class st_render_texture* third_buffer,
		class st_render_texture* depth_buffer,
		class st_render_texture* output_buffer,
		class st_render_texture* output_depth);
	~st_deferred_light_render_pass();

	void render(class st_render_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<class st_framebuffer> _framebuffer = nullptr;
	std::unique_ptr<class st_constant_buffer> _constant_buffer = nullptr;
	std::unique_ptr<class st_buffer> _light_buffer = nullptr;
	std::unique_ptr<class st_resource_table> _resources = nullptr;
};

struct st_deferred_light_cb
{
	st_mat4f _inverse_vp;
	st_vec4f _eye;
	st_vec4f _depth_reconstruction;
};
