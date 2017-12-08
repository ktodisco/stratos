/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_intersection.tests.h"
#include "st_intersection.h"

#include "st_shape.h"

#include <cassert>

void st_intersection_utility_unit_tests()
{
	// Test line to line points.
	{
		st_vec3f start_a = { -1.0f, 1.0f, 0.0f };
		st_vec3f end_a = { 1.0f, 1.0f, 0.0f };
		st_vec3f start_b = { 0.0f, 0.0f, 1.0f };
		st_vec3f end_b = { 0.0f, 0.0f, -1.0f };
		st_vec3f point_a;
		st_vec3f point_b;

		closest_points_on_lines(start_a, end_a, start_b, end_b, point_a, point_b);

		assert(point_a.equal({ 0.0f, 1.0f, 0.0f }));
		assert(point_b.equal({ 0.0f, 0.0f, 0.0f }));
	}

	// Test point to plane distance.
	{
		st_vec3f point = { 0.0f, 25.0f, 0.0f };
		st_plane plane;
		plane._point = { 0.0f, 24.0f, 0.0f };
		plane._normal = st_vec3f::y_vector();

		float dist = distance_to_plane(point, &plane);

		assert(st_equalf(dist, 1.0f));
	}
}

void st_intersection_unit_tests()
{
	// First test sphere collisions.
	{
		st_sphere sphere_a, sphere_b;
		sphere_a._center = { 0.0f, 0.0f, 0.0f };
		sphere_a._radius = 3.0f;
		sphere_b._center = { 0.0f, 0.0f, 0.0f };
		sphere_b._radius = 3.0f;

		st_mat4f trans_a, trans_b;
		trans_a.make_translation({ 4.0f, 0.0f, 0.0f });
		trans_b.make_identity();

		st_collision_info info;
		bool collision = sphere_vs_sphere(&sphere_a, trans_a, &sphere_b, trans_b, &info);
		assert(collision);

		trans_b.translate({ 0.0f, 5.0f, 0.0f });
		collision = sphere_vs_sphere(&sphere_a, trans_a, &sphere_b, trans_b, &info);
		assert(!collision);
	}

	// Now test AABB collisions.
	{
		st_aabb aabb_a, aabb_b;
		aabb_a._min = { -0.5f, -0.5f, -0.5f };
		aabb_a._max = { 0.5f, 0.5f, 0.5f };
		aabb_b._min = { -2.0f, -2.0f, -2.0f };
		aabb_b._max = { 2.0f, 2.0f, 2.0f };

		st_mat4f trans_a, trans_b;
		trans_a.make_identity();
		trans_b.make_translation({ 1.0f, 1.5f, 0.5f });

		st_collision_info info;
		bool collision = aabb_vs_aabb(&aabb_a, trans_a, &aabb_b, trans_b, &info);
		assert(collision);

		trans_a.translate({ 10.0f, -5.0f, -30.0f });
		collision = aabb_vs_aabb(&aabb_a, trans_a, &aabb_b, trans_b, &info);
		assert(!collision);
	}

	// Test OOBB collisions.
	{
		st_oobb oobb_a, oobb_b;
		oobb_a._center = { 0.0f, 0.0f, 0.0f };
		oobb_a._half_vectors[0] = { 0.6f, 0.0f, 0.0f };
		oobb_a._half_vectors[1] = { 0.0f, 1.0f, 0.0f };
		oobb_a._half_vectors[2] = { 0.0f, 0.0f, 0.3f };
		oobb_b._center = { 0.0f, 0.0f, 0.0f };
		oobb_b._half_vectors[0] = { 1.0f, 0.0f, 0.0f };
		oobb_b._half_vectors[1] = { 0.0f, 1.0f, 0.0f };
		oobb_b._half_vectors[2] = { 0.0f, 0.0f, 1.0f };

		st_mat4f trans_a, trans_b;
		trans_a.make_identity();
		trans_b.make_translation({ -1.0f, 0.0f, 0.0f });

		st_collision_info info;
		bool collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(collision);

		st_quatf rotation_q;
		rotation_q.make_axis_angle({ 0.0f, 0.0f, 1.0f }, st_degrees_to_radians(45.0f));
		trans_b.rotate(rotation_q);

		collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(collision);

		rotation_q.make_axis_angle({ 1.0f, 0.0f, 0.0f }, st_degrees_to_radians(90.0f));
		trans_b.rotate(rotation_q);

		collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(collision);

		trans_a.translate({ 0.0f, -5.0f, 0.0f});

		collision = separating_axis_test(&oobb_a, trans_a, &oobb_b, trans_b, &info);
		assert(!collision);
	}

	// TODO: Test GJK for convex hull collisions.
}
