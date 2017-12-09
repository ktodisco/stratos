#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_widget.h"

#include "math/st_vec2f.h"

/*
** GUI pressable checkbox widget.
*/
class st_checkbox : public st_widget
{
public:
	st_checkbox(bool state, const char* text, float x, float y, struct st_frame_params* params);
	~st_checkbox();

	bool get_hover(const struct st_frame_params* params) const;
	bool get_pressed(const struct st_frame_params* params) const;
	bool get_clicked(const struct st_frame_params* params) const;

private:
	st_vec2f _min;
	st_vec2f _max;
};
