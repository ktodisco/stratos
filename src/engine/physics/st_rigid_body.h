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
#include "math/st_vec3f.h"

#include <cstdint>
#include <vector>

enum st_rigid_body_flags
{
	k_static = 1,
	k_weightless = 2,
};

/*
** Represents a body in the physics simulation.
** Static bodies will not move (e.g. the floor).
*/
class st_rigid_body final
{
public:
	st_rigid_body(struct st_shape* shape, float mass);
	~st_rigid_body();

	void get_debug_draw(struct st_dynamic_drawcall* drawcall);

	void make_static();
	void make_weightless();

	void add_linear_velocity(const st_vec3f& v);
	void add_angular_momentum(const st_vec3f& v);

private:
	st_mat4f _transform;
	st_quatf _orientation = { 0.0f, 0.0f, 0.0f, 0.0f };

	st_vec3f _angular_momentum = st_vec3f::zero_vector();
	st_vec3f _angular_velocity = st_vec3f::zero_vector();
	st_vec3f _velocity = st_vec3f::zero_vector();

	st_mat4f _inertia_tensor;

	float _mass;

	// Ordinarily this would live in a collision material structure.
	// We just include the value here for simplicity.
	float _coefficient_of_restitution = 0.5f;

	struct st_shape* _shape;

	std::vector<st_vec3f> _forces;
	std::vector<st_vec3f> _torques;

	uint32_t _flags;

	friend class st_physics_world;
	friend class st_physics_component;
};
