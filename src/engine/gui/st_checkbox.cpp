/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_checkbox.h"
#include "st_font.h"

#include "framework/st_frame_params.h"

st_checkbox::st_checkbox(bool state, const char* text, float x, float y, st_frame_params* params)
{
	st_vec2f text_min, text_max;

	extern st_font* g_font;
	g_font->print(params, text, x + k_checkbox_offset, y, k_text_color, &text_min, &text_max);

	_min = { x, text_min.y };
	_max = { x + (text_max.y - text_min.y), text_max.y };

	draw_outline(params, _min, _max, get_hover(params) ? k_button_hover_color : k_button_color, 0.0f);
	if (state)
	{
		draw_check(params, _min, _max, get_pressed(params) ? k_button_hover_color : k_button_color);
	}
}

st_checkbox::~st_checkbox()
{
}

bool st_checkbox::get_hover(const st_frame_params* params) const
{
	return
		_min.x <= params->_mouse_x &&
		_min.y <= params->_mouse_y &&
		_max.x >= params->_mouse_x &&
		_max.y >= params->_mouse_y;
}

bool st_checkbox::get_pressed(const st_frame_params* params) const
{
	return params->_mouse_press_mask != 0 && get_hover(params);
}

bool st_checkbox::get_clicked(const st_frame_params* params) const
{
	return params->_mouse_click_mask != 0 && get_hover(params);
}
