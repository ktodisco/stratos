/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_button.h>

#include <framework/st_frame_params.h>

#include <gui/st_font.h>

st_button::st_button(const char* text, float x, float y, st_frame_params* params)
{
	extern std::unique_ptr<st_font> g_font;
	g_font->print(params, text, x, y, k_text_color, &_min, &_max);

	if (get_hover(params))
	{
		draw_outline(params, _min, _max, k_button_hover_color, k_button_offset);
	}
	else
	{
		draw_outline(params, _min, _max, k_button_color, k_button_offset);
	}

	if (get_pressed(params))
	{
		draw_fill(params, _min, _max, k_button_press_color);
		g_font->print(params, text, x, y, k_text_color);
	}
}

st_button::~st_button()
{
}

bool st_button::get_hover(const st_frame_params* params) const
{
	return
		_min.x <= params->_mouse_x &&
		_min.y <= params->_mouse_y &&
		_max.x >= params->_mouse_x &&
		_max.y >= params->_mouse_y;
}

bool st_button::get_pressed(const st_frame_params* params) const
{
	return params->_mouse_press_mask != 0 && get_hover(params);
}

bool st_button::get_clicked(const st_frame_params* params) const
{
	return params->_mouse_click_mask != 0 && get_hover(params);
}
