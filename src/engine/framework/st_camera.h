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
	st_camera(const st_vec3f& eye);
	~st_camera();

	void update(struct st_frame_params* params);

	void rotate(const struct st_quatf& rotation);

	const st_mat4f& get_transform() const { return _transform; }
	void set_transform(const st_mat4f& t) { _transform = t; }

private:
	float _yaw = 0.0f;
	float _pitch = 0.0f;
	st_vec3f _position;
	st_mat4f _transform;
};
