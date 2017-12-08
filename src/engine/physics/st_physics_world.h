#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_vec3f.h"

#include <atomic>
#include <vector>

#define st_PHYSICS_DEBUG_DRAW 1

struct st_collision_info;
class st_rigid_body;
struct st_frame_params;

/*
** Represents the physics simulation environment.
** Tracks all rigid bodies and dispatches the physics and collision simulations.
*/
class st_physics_world
{
public:
	st_physics_world();
	~st_physics_world();

	void add_rigid_body(st_rigid_body* body);
	void remove_rigid_body(st_rigid_body* body);

	void step(st_frame_params* params);

private:
	std::vector<st_rigid_body*> _bodies;
	std::atomic_flag _bodies_lock = ATOMIC_FLAG_INIT;

	st_vec3f _gravity;

	void step_linear_dynamics(st_frame_params* params, st_rigid_body* body);
	void step_angular_dynamics(st_frame_params* params, st_rigid_body* body);

	void test_intersections(st_frame_params* params);

	void resolve_collision(st_rigid_body* body_a, st_rigid_body* body_b, st_collision_info* info);
};
