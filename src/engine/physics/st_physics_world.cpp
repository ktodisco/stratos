/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_physics_world.h"
#include "st_intersection.h"
#include "st_rigid_body.h"
#include "st_shape.h"

#include "framework/st_drawcall.h"
#include "framework/st_frame_params.h"

#include <algorithm>
#include <assert.h>
#include <ctime>

typedef bool (*intersection_func_t)(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

static intersection_func_t k_dispatch_table[k_shape_count][k_shape_count];

st_physics_world::st_physics_world()
{
	// Clear the dispatch table.
	for (int i = 0; i < k_shape_count; ++i)
	{
		for (int j = 0; j < k_shape_count; ++j)
		{
			k_dispatch_table[i][j] = intersection_unimplemented;
		}
	}

	k_dispatch_table[k_shape_sphere][k_shape_sphere] = sphere_vs_sphere;
	k_dispatch_table[k_shape_oobb][k_shape_oobb] = separating_axis_test;
	k_dispatch_table[k_shape_plane][k_shape_oobb] = oobb_vs_plane;
	k_dispatch_table[k_shape_oobb][k_shape_plane] = oobb_vs_plane;
	k_dispatch_table[k_shape_plane][k_shape_sphere] = sphere_vs_plane;
	k_dispatch_table[k_shape_sphere][k_shape_plane] = sphere_vs_plane;

	// Default gravity to Earth's constant.
	_gravity = { 0.0f, -9.807f, 0.0f };
}

st_physics_world::~st_physics_world()
{
	assert(_bodies.size() == 0);
}

void st_physics_world::add_rigid_body(st_rigid_body* body)
{
	while (_bodies_lock.test_and_set(std::memory_order_acquire)) {}
	_bodies.push_back(body);
	_bodies_lock.clear(std::memory_order_release);
}

void st_physics_world::remove_rigid_body(st_rigid_body* body)
{
	while (_bodies_lock.test_and_set(std::memory_order_acquire)) {}
	_bodies.erase(std::remove(_bodies.begin(), _bodies.end(), body));
	_bodies_lock.clear(std::memory_order_release);
}

void st_physics_world::step(st_frame_params* params)
{
	while (_bodies_lock.test_and_set(std::memory_order_acquire)) {}

	// Step the physics sim.
	for (int i = 0; i < _bodies.size(); ++i)
	{
		if (_bodies[i]->_flags & k_static) continue;

		st_rigid_body* body = _bodies[i];

		if ((_bodies[i]->_flags & k_weightless) == 0)
		{
			body->_forces.push_back(_gravity);
		}

		step_linear_dynamics(params, body);
		step_angular_dynamics(params, body);
	}

	test_intersections(params);

	_bodies_lock.clear(std::memory_order_release);
}

void st_physics_world::test_intersections(st_frame_params* params)
{
	// Intersection tests. Naive N^2 comparisons.
	for (int i = 0; i < _bodies.size(); ++i)
	{
		for (int j = i + 1; j < _bodies.size(); ++j)
		{
			st_shape* shape_a = _bodies[i]->_shape;
			st_shape* shape_b = _bodies[j]->_shape;
			intersection_func_t func = k_dispatch_table[shape_a->get_type()][shape_b->get_type()];

			st_collision_info info;
			bool collision = func(shape_a, _bodies[i]->_transform, shape_b, _bodies[j]->_transform, &info);
			if (collision)
			{
				std::time_t time = std::chrono::system_clock::to_time_t(
					std::chrono::system_clock::now());

#if defined(st_PHYSICS_DEBUG_DRAW)
				st_dynamic_drawcall collision_draw;
				collision_draw._positions.push_back(st_vec3f::zero_vector());
				collision_draw._positions.push_back(info._normal);
				collision_draw._indices.push_back(0);
				collision_draw._indices.push_back(1);
				collision_draw._color = { 1.0f, 1.0f, 0.0f };
				collision_draw._draw_mode = GL_LINES;
				collision_draw._material = nullptr;
				collision_draw._transform.make_translation(info._point);

				while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
				params->_dynamic_drawcalls.push_back(collision_draw);
				params->_dynamic_drawcall_lock.clear(std::memory_order_release);
#endif
				// We should not attempt to resolve collisions if we're paused and have not single stepped.
				bool should_resolve = params->_delta_time > std::chrono::milliseconds(0) || params->_single_step;

				if (should_resolve)
				{
					resolve_collision(_bodies[i], _bodies[j], &info);
				}
			}
		}
	}
}

void st_physics_world::step_linear_dynamics(st_frame_params* params, st_rigid_body* body)
{
	// Linear dynamics.
	st_vec3f overall_force = st_vec3f::zero_vector();
	while (body->_forces.size() > 0)
	{
		overall_force += body->_forces.back();
		body->_forces.pop_back();
	}

	// Integrate using 4th order Runge-Kutta numerical integration.
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	st_vec3f position = body->_transform.get_translation();
	st_vec3f p1 = position;
	st_vec3f v1 = body->_velocity;

	st_vec3f p2 = position + v1.scale_result(0.5f * dt);
	st_vec3f v2 = body->_velocity + overall_force.scale_result(0.5f * dt);

	st_vec3f p3 = position + v2.scale_result(0.5f * dt);
	st_vec3f v3 = body->_velocity + overall_force.scale_result(0.5f * dt);

	st_vec3f p4 = position + v3.scale_result(dt);
	st_vec3f v4 = body->_velocity + overall_force.scale_result(dt);

	st_vec3f new_position = position + (v1 + v2.scale_result(2.0f) + v3.scale_result(2.0f) + v4).scale_result(dt / 6.0f);
	st_vec3f new_velocity = body->_velocity + (overall_force.scale_result(6.0f)).scale_result(dt / 6.0f);

	body->_velocity = new_velocity;
	body->_transform.set_translation(new_position);
}

void st_physics_world::step_angular_dynamics(st_frame_params* params, st_rigid_body* body)
{
	// Save the translation.
	st_vec3f translation = body->_transform.get_translation();

	// Angular dynamics.
	st_vec3f overall_torque = st_vec3f::zero_vector();
	while (body->_torques.size() > 0)
	{
		overall_torque += body->_torques.back();
		body->_torques.pop_back();
	}
	
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	body->_angular_momentum += overall_torque.scale_result(dt);
	st_mat4f inertia_tensor_inv = body->_inertia_tensor;
	inertia_tensor_inv.invert();
	body->_angular_velocity = inertia_tensor_inv.transform_vector(body->_angular_momentum);
	st_quatf ang_velocity = { body->_angular_velocity.x, body->_angular_velocity.y, body->_angular_velocity.z, 0.0f };
	body->_orientation += (ang_velocity * body->_orientation).scale_result(0.5f * dt);
	body->_orientation.normalize();

	// Assemble the new transform.
	body->_transform.make_rotation(body->_orientation);

	// Restore the translation.
	body->_transform.set_translation(translation);
}

void st_physics_world::resolve_collision(st_rigid_body* body_a, st_rigid_body* body_b, st_collision_info* info)
{
	// Calculate the velocities of A and B at the point of collision.
	st_vec3f r_ap = body_a->_shape->get_offset_to_point(body_a->_transform, info->_point);
	st_vec3f r_bp = body_b->_shape->get_offset_to_point(body_b->_transform, info->_point);

	st_vec3f angular_velocity_a = st_vec3f_cross(body_a->_angular_velocity, r_ap);
	st_vec3f angular_velocity_b = st_vec3f_cross(body_b->_angular_velocity, r_bp);

	st_vec3f velocity_a = body_a->_velocity + angular_velocity_a;
	st_vec3f velocity_b = body_b->_velocity + angular_velocity_b;
	
	// First move the objects so they no longer intersect.
	// Each object will be moved proportionally to their incoming velocities.
	// If an object is static, it won't be moved.
	float total_velocity = velocity_a.mag() + velocity_b.mag();
	float percentage_a = (body_a->_flags & k_static) ? 0.0f : velocity_a.mag() / total_velocity;
	float percentage_b = (body_b->_flags & k_static) ? 0.0f : velocity_b.mag() / total_velocity;

	if ((body_a->_flags & k_static) == 0 && velocity_a.mag2() > 0.0f)
	{
		float pen_a = info->_penetration * percentage_a + 0.001f;
		body_a->_transform.set_translation(body_a->_transform.get_translation() - velocity_a.normal().scale_result(pen_a));
	}
	if ((body_b->_flags & k_static) == 0 && velocity_b.mag2() > 0.0f)
	{
		float pen_b = info->_penetration * percentage_b + 0.001f;
		body_b->_transform.set_translation(body_b->_transform.get_translation() - velocity_b.normal().scale_result(pen_b));
	}

	// Average the coefficients of restitution.
	float cor_average = (body_a->_coefficient_of_restitution + body_b->_coefficient_of_restitution) / 2.0f;
	float one_over_mass_a = 1.0f / body_a->_mass;
	float one_over_mass_b = 1.0f / body_b->_mass;

	// Now, calculate impulse j.
	float j = 0.0f;
	if (body_a->_flags & k_static)
	{
		float numerator = -velocity_b.dot(info->_normal) * (1 + cor_average);
		float denominator = one_over_mass_b + st_vec3f_cross(body_b->_inertia_tensor.inverse().transform_vector(st_vec3f_cross(r_bp, info->_normal)), r_bp).dot(info->_normal);
		j = numerator / denominator;
	}
	else if (body_b->_flags & k_static)
	{
		float numerator = -velocity_a.dot(info->_normal) * (1 + cor_average);
		float denominator = one_over_mass_a + st_vec3f_cross(body_a->_inertia_tensor.inverse().transform_vector(st_vec3f_cross(r_ap, info->_normal)), r_ap).dot(info->_normal);
		j = numerator / denominator;
	}
	else
	{
		st_vec3f a_ang_denom = body_a->_inertia_tensor.inverse().transform_vector(st_vec3f_cross(r_ap, info->_normal));
		a_ang_denom = st_vec3f_cross(a_ang_denom, r_ap);
		st_vec3f b_ang_denom = body_b->_inertia_tensor.inverse().transform_vector(st_vec3f_cross(r_bp, info->_normal));
		b_ang_denom = st_vec3f_cross(b_ang_denom, r_bp);

		float denominator = one_over_mass_a + one_over_mass_b + info->_normal.dot(a_ang_denom + b_ang_denom);
		float numerator = (1 + cor_average) * (velocity_b.dot(info->_normal) - velocity_a.dot(info->_normal));

		j = numerator / denominator;
	}

	st_vec3f impulse = info->_normal.scale_result(j);

	if ((body_a->_flags & k_static) == 0)
	{
		body_a->_velocity += impulse.scale_result(1.0f / body_a->_mass);
		body_a->_angular_momentum -= body_a->_inertia_tensor.inverse().transform_vector(st_vec3f_cross(impulse, r_ap));
	}

	if ((body_b->_flags & k_static) == 0)
	{
		body_b->_velocity -= impulse.scale_result(1.0f / body_b->_mass);
		body_b->_angular_momentum += body_b->_inertia_tensor.inverse().transform_vector(st_vec3f_cross(impulse, r_bp));
	}
}
