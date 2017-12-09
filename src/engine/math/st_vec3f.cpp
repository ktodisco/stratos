/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_vec3f.h"

static st_vec3f vec3f_zero = { 0.0f, 0.0f, 0.0f };
static st_vec3f vec3f_one = { 1.0f, 1.0f, 0.0f };
static st_vec3f vec3f_x = { 1.0f, 0.0f, 0.0f };
static st_vec3f vec3f_y = { 0.0f, 1.0f, 0.0f };
static st_vec3f vec3f_z = { 0.0f, 0.0f, 1.0f };

st_vec3f st_vec3f::zero_vector()
{
	return vec3f_zero;
}

st_vec3f st_vec3f::one_vector()
{
	return vec3f_one;
}

st_vec3f st_vec3f::x_vector()
{
	return vec3f_x;
}

st_vec3f st_vec3f::y_vector()
{
	return vec3f_y;
}

st_vec3f st_vec3f::z_vector()
{
	return vec3f_z;
}
