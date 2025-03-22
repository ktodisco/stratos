#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_mat4f.h"

/*
** Represents the a camera used to navigate the simulation world.
*/
class st_camera
{
public:
	// The camera assumes an initial direction along the positive z axis, and
	// up vector along the positive y.
	st_camera(const st_vec3f& eye, uint32_t width, uint32_t height);
	~st_camera();

	void update(struct st_frame_params* params);
	void debug();
	void resize(uint32_t width, uint32_t height) { _width = width; _height = height; }

	void set_yaw(float yaw) { _yaw = yaw; }
	void set_pitch(float pitch) { _pitch = pitch; }

	const st_mat4f& get_transform() const { return _transform; }
	void set_transform(const st_mat4f& t) { _transform = t; }

private:
	float _yaw = 0.0f;
	float _pitch = 0.0f;
	uint32_t _width = 0;
	uint32_t _height = 0;
	st_vec3f _position = st_vec3f::zero_vector();
	st_mat4f _transform = {};

	// Both are expressed in mm.
	float _focal_length = 400.0f;
	float _diameter = 25.0f;

	float _speed = 0.01f;
	float _iso = 100.0f;
};
