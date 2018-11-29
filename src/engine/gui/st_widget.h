#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <math/st_vec3f.h>

#include <graphics/material/st_constant_color_material.h>

#include <memory>
#include <vector>

/*
** Base class for GUI widgets.
*/
class st_widget
{
public:
	st_widget();
	virtual ~st_widget();

protected:
	static const st_vec3f k_button_color;
	static const st_vec3f k_button_hover_color;
	static const st_vec3f k_button_press_color;
	static const st_vec3f k_text_color;
	static const float k_button_offset;
	static const float k_checkbox_offset;

	void draw_outline(struct st_frame_params* params, const struct st_vec2f& min, const struct st_vec2f& max, const struct st_vec3f& color, float offset);
	void draw_check(struct st_frame_params* params, const struct st_vec2f& min, const struct st_vec2f& max, const struct st_vec3f& color);
	void draw_fill(struct st_frame_params* params, const struct st_vec2f& min, const struct st_vec2f& max, const struct st_vec3f& color);

private:
	std::vector<std::unique_ptr<st_constant_color_material>> _materials;
};
