#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#define _USE_MATH_DEFINES
#include <algorithm>
#include <math.h>

#define st_absf fabsf
#define st_cosf cosf
#define st_powf powf
#define st_sinf sinf
#define st_sqrtf sqrtf
#define st_tanf tanf
#define st_max std::max
#define st_min std::min

#define st_PI ((float)M_PI)

/*
** Convert degrees to radians.
*/
inline float st_degrees_to_radians(float degrees)
{
	return (degrees * st_PI) / 180.0f;
}

/*
** Determine if two floats are largely equivalent.
*/
inline bool st_equalf(float a, float b)
{
	float diff = st_absf(a - b);
	return diff < 0.0000005f || diff < st_absf(a * 0.0000005f) || diff < st_absf(b * 0.0000005f);
}
