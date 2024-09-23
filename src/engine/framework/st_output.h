#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "graphics/st_drawcall.h"
#include "math/st_mat4f.h"

#include <memory>
#include <vector>

class st_window;

/*
** Represents the output stage of the frame.
** Owns whatever is drawn on the screen.
*/
class st_output
{
public:

	st_output(const st_window* window, class st_graphics_context* render_context);
	~st_output();

	void update(struct st_frame_params* params);

private:

	const st_window* _window;
	class st_graphics_context* _graphics_context;

	std::unique_ptr<class st_gbuffer_render_pass> _gbuffer_pass;
	std::unique_ptr<class st_deferred_light_render_pass> _deferred_pass;
	std::unique_ptr<class st_bloom_render_pass> _bloom_pass;
	std::unique_ptr<class st_tonemap_render_pass> _tonemap_pass;
	std::unique_ptr<class st_passthrough_render_pass> _passthrough_pass;
	std::unique_ptr<class st_ui_render_pass> _ui_pass;

	std::unique_ptr<class st_render_texture> _gbuffer_albedo_target;
	std::unique_ptr<class st_render_texture> _gbuffer_normal_target;
	std::unique_ptr<class st_render_texture> _gbuffer_third_target;
	std::unique_ptr<class st_render_texture> _depth_stencil_target;

	std::unique_ptr<class st_render_texture> _deferred_target;
	std::unique_ptr<class st_render_texture> _deferred_depth;

	std::unique_ptr<class st_render_texture> _bloom_target;

	std::unique_ptr<class st_render_texture> _tonemap_target;
};
