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

struct st_dynamic_drawcall;

enum st_shape_t
{
	k_shape_plane,
	k_shape_sphere,
	k_shape_aabb,
	k_shape_oobb,
	k_shape_convex_hull,
	k_shape_count,
};

/*
** The pure virtual base shape type.
** This allows a rigid body to store an arbitrary shape with a single pointer type.
**
** Shape dimensions are always specified in local object space, as if the shape is
** being placed on an object regardless of the position of the object.
** So, for example, a sphere shape with center (0,0,0) and radius 5 will be centered
** on an object's local origin and surround it 5 units in all directions.
**
** Similarly, an axis-aligned bounding box with a min extent of (-1,-1,-1) and max
** extent of (1,1,1), the box will be sized 2x2x2 and will be centered on the
** object's local origin.
*/
struct st_shape
{
	/*
	** Returns the type of the shape.
	*/
	virtual st_shape_t get_type() const = 0;

	/*
	** Fills out a debug draw call with the geometry of the collision shape.
	*/
	virtual void get_debug_draw(const st_mat4f& transform, struct st_dynamic_drawcall* drawcall) = 0;

	/*
	** Fills out the inertia tensor matrix for the shape, given the mass.
	*/
	virtual void get_inertia_tensor(st_mat4f& tensor, float mass) = 0;

	/*
	** Returns the vector from the center of mass to the point in space.
	*/
	virtual st_vec3f get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const = 0;
};

/*
** Defines a collidable plane with a point on the plane and a plane normal.
*/
struct st_plane final : st_shape
{
	st_vec3f _point;
	st_vec3f _normal;

	st_shape_t get_type() const override { return k_shape_plane; }
	void get_debug_draw(const st_mat4f& transform, struct st_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(st_mat4f& tensor, float mass) override;
	st_vec3f get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const override;
};

/*
** Defines a collidable sphere with a center point and radius.
*/
struct st_sphere final : st_shape
{
	st_vec3f _center;
	float _radius;

	st_shape_t get_type() const override { return k_shape_sphere; }
	void get_debug_draw(const st_mat4f& transform, struct st_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(st_mat4f& tensor, float mass) override;
	st_vec3f get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const override;
};

/*
** Defines a collideable axis-aligned bounding box using minimum and maximum extents.
*/
struct st_aabb final : st_shape
{
	st_vec3f _min;
	st_vec3f _max;

	st_shape_t get_type() const override { return k_shape_aabb; }
	void get_debug_draw(const st_mat4f& transform, struct st_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(st_mat4f& tensor, float mass) override;
	st_vec3f get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const override;
};

/*
** Defines a collidable object-oriented bounding box with a center point and
** three half-vectors that define its extents.
*/
struct st_oobb final : st_shape
{
	st_vec3f _center;
	st_vec3f _half_vectors[3];

	st_shape_t get_type() const override { return k_shape_oobb; }
	void get_debug_draw(const st_mat4f& transform, struct st_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(st_mat4f& tensor, float mass) override;
	st_vec3f get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const override;

	void get_corners(std::vector<st_vec3f>& corners) const;
};

/*
** Defines a collidable arbitrary convex hull made up of individual points.
*/
struct st_convex_hull final : st_shape
{
	std::vector<st_vec3f> _positions;

	st_shape_t get_type() const override { return k_shape_convex_hull; }
	void get_debug_draw(const st_mat4f& transform, struct st_dynamic_drawcall* drawcall) override;
	void get_inertia_tensor(st_mat4f& tensor, float mass) override;
	st_vec3f get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const override;
};
