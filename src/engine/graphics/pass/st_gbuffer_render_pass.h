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
class st_gbuffer_render_pass
{
public:
	st_gbuffer_render_pass(
		class st_render_texture* albedo_buffer,
		class st_render_texture* normal_buffer,
		class st_render_texture* third_buffer,
		class st_render_texture* depth_buffer);
	~st_gbuffer_render_pass();

	void render(class st_render_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<class st_vertex_format> _vertex_format = nullptr;

	std::unique_ptr<class st_gbuffer_material> _default_gbuffer = nullptr;
	std::unique_ptr<class st_pipeline_state> _gbuffer_state = nullptr;

	std::unique_ptr<class st_framebuffer> _framebuffer = nullptr;
};
