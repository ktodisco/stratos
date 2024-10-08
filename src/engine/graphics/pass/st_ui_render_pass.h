#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <memory>
#include <vector>

/*
** A render pass that draws the UI elements.
*/
class st_ui_render_pass
{
public:
	st_ui_render_pass();
	~st_ui_render_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	void draw_dynamic(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const struct st_mat4f& proj,
		const struct st_mat4f& view);

	std::unique_ptr<class st_constant_color_material> _default_material = nullptr;
	std::unique_ptr<struct st_pipeline> _default_state = nullptr;

	std::unique_ptr<class st_font_material> _font_material = nullptr;
	std::unique_ptr<struct st_pipeline> _font_state = nullptr;

	std::unique_ptr<struct st_vertex_format> _vertex_format = nullptr;

	std::unique_ptr<struct st_render_pass> _pass = nullptr;
};
