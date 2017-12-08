#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_mat4f.h"

#include <vector>

/*
** Entity object.
** A bucket of components in 3D space. No classes should derive from here.
** All functionality should be in components.
** @see st_component
*/
class st_entity final
{
public:
	st_entity();
	~st_entity();

	void add_component(class st_component* comp);

	void update(struct st_frame_params* params);
	void late_update(struct st_frame_params* params);

	void translate(const struct st_vec3f& translation);
	void rotate(const struct st_quatf& rotation);
	void scale(float s);

	const st_mat4f& get_transform() const { return _transform; }
	void set_transform(const st_mat4f& t) { _transform = t; }

private:
	std::vector<class st_component*> _components;
	st_mat4f _transform;
};
