#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <math/st_mat4f.h>
#include <math/st_vec4f.h>

#include <memory>

struct st_deferred_light_cb
{
	st_mat4f _inverse_vp;
	st_vec4f _eye;
	st_vec4f _depth_reconstruction;

	st_vec4f _sun_direction_power = st_vec4f::zero_vector();
	st_vec4f _sun_color = st_vec4f::zero_vector();
	st_mat4f _sun_vp;
	st_vec4f _sun_shadow_dim;
};

class st_deferred_light_render_pass : public st_fullscreen_render_pass
{
public:
	st_deferred_light_render_pass(
		class st_render_texture* albedo_buffer,
		class st_render_texture* normal_buffer,
		class st_render_texture* third_buffer,
		class st_render_texture* depth_buffer,
		class st_render_texture* directional_shadow_map,
		class st_render_texture* output_buffer);
	~st_deferred_light_render_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<struct st_render_pass> _pass = nullptr;
	std::unique_ptr<struct st_buffer> _constant_buffer = nullptr;
	std::unique_ptr<struct st_buffer> _light_buffer = nullptr;

	st_deferred_light_cb _cb;
};
