#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <math/st_mat4f.h>
#include <math/st_vec3f.h>
#include <math/st_vec4f.h>

struct st_directional_light
{
	st_directional_light(
		st_vec3f direction,
		st_vec3f color,
		float power) : _direction(direction), _color(color), _power(power) {}
	~st_directional_light() {}

	st_vec3f _direction;
	st_vec3f _color;

	// In lux.
	float _power = 0;
};
