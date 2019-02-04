#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <math/st_vec3f.h>

struct st_sphere_light
{
	st_sphere_light(
		st_vec3f position,
		st_vec3f color,
		float power,
		float radius) : _position(position), _color(color), _power(power), _radius(radius) {}

	st_vec3f _position;
	st_vec3f _color;

	// In lumens.
	float _power = 0;

	float _radius = 1.0f;
};

struct st_sphere_light_data
{
	st_vec4f _position_power;
	st_vec4f _color_radius;
};
