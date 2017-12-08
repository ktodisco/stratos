/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_vec4f.h"

static st_vec4f vec4f_zero = { 0.0f, 0.0f, 0.0f, 0.0f };
static st_vec4f vec4f_one = {  1.0f, 1.0f, 1.0f, 1.0f };
static st_vec4f vec4f_x = { 1.0f, 0.0f, 0.0f, 0.0f };
static st_vec4f vec4f_y = { 0.0f, 1.0f, 0.0f, 0.0f };
static st_vec4f vec4f_z = { 0.0f, 0.0f, 1.0f, 0.0f };
static st_vec4f vec4f_w = { 0.0f, 0.0f, 0.0f, 1.0f };

st_vec4f st_vec4f::zero_vector()
{
	return vec4f_zero;
}

st_vec4f st_vec4f::one_vector()
{
	return vec4f_one;
}

st_vec4f st_vec4f::x_vector()
{
	return vec4f_x;
}

st_vec4f st_vec4f::y_vector()
{
	return vec4f_y;
}

st_vec4f st_vec4f::z_vector()
{
	return vec4f_z;
}

st_vec4f st_vec4f::w_vector()
{
	return vec4f_w;
}
