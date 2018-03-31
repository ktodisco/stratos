/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <math/st_vec4f.h>

#include <math/st_vec3f.h>

static st_vec4f vec4f_zero = { 0.0f, 0.0f, 0.0f, 0.0f };
static st_vec4f vec4f_one = {  1.0f, 1.0f, 1.0f, 1.0f };
static st_vec4f vec4f_x = { 1.0f, 0.0f, 0.0f, 0.0f };
static st_vec4f vec4f_y = { 0.0f, 1.0f, 0.0f, 0.0f };
static st_vec4f vec4f_z = { 0.0f, 0.0f, 1.0f, 0.0f };
static st_vec4f vec4f_w = { 0.0f, 0.0f, 0.0f, 1.0f };

st_vec4f::st_vec4f()
{
}

st_vec4f::st_vec4f(float nx, float ny, float nz, float nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}

st_vec4f::st_vec4f(st_vec3f vec3, float nw)
{
	x = vec3.x;
	y = vec3.y;
	z = vec3.z;
	w = nw;
}

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
