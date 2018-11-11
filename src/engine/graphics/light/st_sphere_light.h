#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <math/st_vec3f.h>

class st_sphere_light
{
public:
	st_sphere_light(
		st_vec3f position,
		st_vec3f color,
		float power,
		float radius) : _position(position), _color(color), _power(power), _radius(radius) {}
	~st_sphere_light() {}

	st_vec3f get_position() const { return _position; }
	st_vec3f get_color() const { return _color; }
	float get_power() const { return _power; }
	float get_radius() const { return _radius; }

	void set_position(const st_vec3f& position) { _position = position; }
	void set_radius(const float radius) { _radius = radius; }

private:
	st_vec3f _position;
	st_vec3f _color;

	// In lumens.
	float _power = 0;

	float _radius = 1.0f;
};

struct st_sphere_light_cb
{
	st_mat4f _inverse_vp;
	st_vec4f _eye;
	st_vec4f _position;
	st_vec4f _color;
	st_vec2f _properties;
};
