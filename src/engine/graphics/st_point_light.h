#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <math/st_vec3f.h>

class st_point_light
{
public:
	st_point_light(
		st_vec3f position,
		st_vec3f color,
		float power) : _position(position), _color(color), _power(power) {}
	~st_point_light() {}

	st_vec3f get_position() const { return _position; }
	st_vec3f get_color() const { return _color; }
	float get_power() const { return _power; }

private:
	st_vec3f _position;
	st_vec3f _color;
	
	// In Watts.
	float _power = 0;
};

struct st_point_light_cb
{
	st_mat4f _inverse_vp;
	st_vec4f _eye;
	st_vec4f _position;
	st_vec4f _color;
	float _power;
};
