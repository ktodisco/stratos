#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <memory>
#include <vector>

/*
** A render pass that draws the static and dynamic scene objects.
*/
class st_scene_render_pass
{
public:
	st_scene_render_pass();
	~st_scene_render_pass();

	void render(class st_render_context* context, const struct st_frame_params* params);

private:
	void draw_dynamic(
		class st_render_context* context,
		const struct st_frame_params* params,
		const class st_mat4f& proj,
		const class st_mat4f& view);

	std::unique_ptr<class st_vertex_format> _vertex_format = nullptr;

	std::unique_ptr<class st_phong_material> _default_phong = nullptr;
	std::unique_ptr<class st_pipeline_state> _phong_state = nullptr;

	std::unique_ptr<class st_unlit_texture_material> _default_unlit_texture = nullptr;
	std::unique_ptr<class st_pipeline_state> _unlit_texture_state = nullptr;
};
