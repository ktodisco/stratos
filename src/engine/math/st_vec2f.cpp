/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_vec2f.h"

static st_vec2f vec2f_zero = { 0.0f, 0.0f };
static st_vec2f vec2f_one = { 1.0f, 1.0f };
static st_vec2f vec2f_x = { 1.0f, 0.0f };
static st_vec2f vec2f_y = { 0.0f, 1.0f };

st_vec2f st_vec2f::zero_vector()
{
	return vec2f_zero;
}

st_vec2f st_vec2f::one_vector()
{
	return vec2f_one;
}

st_vec2f st_vec2f::x_vector()
{
	return vec2f_x;
}

st_vec2f st_vec2f::y_vector()
{
	return vec2f_y;
}
