/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_slider.h"

#include <framework/st_frame_params.h>

#include <gui/st_font.h>

st_slider::st_slider(float x, float y, float min_val, float max_val, float* current, struct st_frame_params* params)
{
	// Draw the slider line.
	const float k_slider_width = 100.0f;
	draw_outline(params, st_vec2f{ x, y + 2.0f }, st_vec2f{ x + k_slider_width, y + 2.0f }, k_button_color, 0.0f);

	// Find the bounds of the slider control point and draw it.
	float ratio = (*current) / (min_val + max_val);
	_min = st_vec2f{ x + k_slider_width * ratio - 5.0f, y - 5.0f };
	_max = st_vec2f{ x + k_slider_width * ratio + 5.0f, y + 5.0f };

	auto fill_color = get_hover(params) ? k_button_hover_color : k_button_color;
	draw_fill(params, _min, _max, fill_color);

	if (get_pressed(params))
	{
		float mouse_x = params->_mouse_delta_x;
		float single_tick = (max_val - min_val) / k_slider_width;

		*current += single_tick * mouse_x;

		*current = max(min_val, min(max_val, *current)); 
	}
}

st_slider::~st_slider()
{
}

bool st_slider::get_hover(const struct st_frame_params* params) const
{
	return
		_min.x <= params->_mouse_x &&
		_min.y <= params->_mouse_y &&
		_max.x >= params->_mouse_x &&
		_max.y >= params->_mouse_y;
}

bool st_slider::get_pressed(const struct st_frame_params* params) const
{
	return params->_mouse_press_mask != 0 && get_hover(params);
}
