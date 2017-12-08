#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_widget.h"

#include "math/st_vec2f.h"

/*
** GUI pressable button widget.
*/
class st_button : public st_widget
{
public:
	st_button(const char* text, float x, float y, struct st_frame_params* params);
	~st_button();

	bool get_hover(const struct st_frame_params* params) const;
	bool get_pressed(const struct st_frame_params* params) const;
	bool get_clicked(const struct st_frame_params* params) const;

private:
	st_vec2f _min;
	st_vec2f _max;
};
