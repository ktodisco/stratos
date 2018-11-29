#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_widget.h"

#include "math/st_vec2f.h"

/*
** GUI slider to lerp between two values.
*/
class st_slider : public st_widget
{
public:
	st_slider(float x, float y, float min_val, float max_val, float* current, struct st_frame_params* params);
	~st_slider();

	bool get_hover(const struct st_frame_params* params) const;
	bool get_pressed(const struct st_frame_params* params) const;

private:
	st_vec2f _min;
	st_vec2f _max;
};
