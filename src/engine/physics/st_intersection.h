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

#include <vector>

struct st_shape;
struct st_plane;
class st_rigid_body;

/*
** Information returned when a collision is detected.
** Includes the point of collision, the normal at the collision point, and
** the amount the two objects are interpenetrating.
*/
struct st_collision_info
{
	st_vec3f _point;
	st_vec3f _normal;
	float _penetration;
};

/*
** Compute the distance from a point in 3d space to a plane.
*/
float distance_to_plane(const st_vec3f& point, const st_plane* plane);

/*
** Compute the distance from a point in 3d space to a line segment.
*/
float distance_to_line_segment(const st_vec3f& point, const st_vec3f& a, const st_vec3f& b);

/*
** Compute the closest points on two lines.
** @returns True if the points lie with the bounds provided for each segment.
*/
bool closest_points_on_lines(
	const st_vec3f& start_a,
	const st_vec3f& end_a,
	const st_vec3f& start_b,
	const st_vec3f& end_b,
	st_vec3f& point_a,
	st_vec3f& point_b);

/*
** Compute the point farthest along a directional vector.
*/
st_vec3f farthest_along_vector(const std::vector<st_vec3f>& points, const st_vec3f& vector);

/*
** Stub function for unimplemented collision algorithms.
*/
bool intersection_unimplemented(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

/*
** Check for a collision between sphere and plane.
*/
bool sphere_vs_plane(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

/*
** Check for a collision between bounding box and plane.
*/
bool oobb_vs_plane(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

/*
** Check for a collision between two sphere shapes.
*/
bool sphere_vs_sphere(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

/*
** Check for a collision between two capsule shapes.
*/
bool capsule_vs_capsule(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

/*
** Check for a collision between two axis-aligned bounding boxes.
*/
bool aabb_vs_aabb(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

/*
** Check for a collision between two oriented bounding boxes.
*/
bool separating_axis_test(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);

/*
** Check for a collision between two arbitrary convex hulls.
*/
bool gjk(const st_shape* a, const st_mat4f& transform_a, const st_shape* b, const st_mat4f& transform_b, st_collision_info* info);
