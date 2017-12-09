#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <vector>

/*
** A render pass that draws the UI elements.
*/
class st_ui_render_pass
{
public:
	st_ui_render_pass();
	~st_ui_render_pass();

	void render(const struct st_frame_params* params);

private:
	void draw_dynamic(
		const std::vector<struct st_dynamic_drawcall>& drawcalls,
		const class st_mat4f& proj,
		const class st_mat4f& view);

	class st_constant_color_material* _default_material;
};
