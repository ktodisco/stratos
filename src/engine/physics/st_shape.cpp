/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_shape.h"
#include "framework/st_drawcall.h"
#include "graphics/st_debug_geometry.h"
#include "math/st_math.h"

#include <vector>

void st_plane::get_debug_draw(const st_mat4f& transform, st_dynamic_drawcall* drawcall)
{
	st_vec3f position = transform.get_translation() + _point;

	// Get a vector perpendicular to the plane normal.
	st_vec3f perp;
	if (_normal.z < _normal.x) perp = { _normal.y, -_normal.x, 0 };
	else perp = { 0, -_normal.z, _normal.y };

	// Get two other perpendicular vectors to make our plane points.
	st_vec3f vec1 = st_vec3f_cross(_normal, perp).normal();
	st_vec3f vec2 = st_vec3f_cross(_normal, vec1).normal();

	const float k_plane_size = 10.0f;
	vec1.scale(k_plane_size);
	vec2.scale(k_plane_size);

	drawcall->_positions.push_back(-vec1 - vec2);
	drawcall->_positions.push_back(-vec1 + vec2);
	drawcall->_positions.push_back(vec1 + vec2);
	drawcall->_positions.push_back(vec1 - vec2);

	uint32_t indices[] =
	{
		0, 2, 1,
		0, 3, 2,
	};

	for (int i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i)
	{
		drawcall->_indices.push_back(indices[i]);
	}

	drawcall->_color = { 0.2f, 0.2f, 0.2f };
	drawcall->_draw_mode = GL_TRIANGLES;
	drawcall->_transform = transform;
	drawcall->_material = nullptr;
}

void st_plane::get_inertia_tensor(st_mat4f& tensor, float mass)
{
	// Unimplemented.
}

st_vec3f st_plane::get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const
{
	// Unimplemented.
	return st_vec3f::zero_vector();
}

void st_sphere::get_debug_draw(const st_mat4f& transform, st_dynamic_drawcall* drawcall)
{
	draw_debug_sphere(_radius, transform, drawcall);
}

void st_sphere::get_inertia_tensor(st_mat4f& tensor, float mass)
{
	// Unimplemented.
}

st_vec3f st_sphere::get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const
{
	st_vec3f center = transform.transform_point(_center);
	return point - center;
}

void st_aabb::get_debug_draw(const st_mat4f& transform, st_dynamic_drawcall* drawcall)
{
	drawcall->_positions.push_back({ _min.x, _min.y, _min.z });
	drawcall->_positions.push_back({ _min.x, _min.y, _max.z });
	drawcall->_positions.push_back({ _min.x, _max.y, _min.z });
	drawcall->_positions.push_back({ _min.x, _max.y, _max.z });
	drawcall->_positions.push_back({ _max.x, _min.y, _min.z });
	drawcall->_positions.push_back({ _max.x, _min.y, _max.z });
	drawcall->_positions.push_back({ _max.x, _max.y, _min.z });
	drawcall->_positions.push_back({ _max.x, _max.y, _max.z });

	uint32_t indices[] =
	{
		0, 1,
		1, 3,
		3, 2,
		2, 0,
		0, 4,
		4, 6,
		6, 2,
		4, 5,
		5, 7,
		7, 6,
		3, 7,
		1, 5,
	};

	for (int i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i)
	{
		drawcall->_indices.push_back(indices[i]);
	}

	drawcall->_color = { 0.0f, 1.0f, 0.0f };
	drawcall->_draw_mode = GL_LINES;
	drawcall->_transform = transform;
	drawcall->_material = nullptr;
}

void st_aabb::get_inertia_tensor(st_mat4f& tensor, float mass)
{
	// Unimplemented.
}

st_vec3f st_aabb::get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const
{
	st_vec3f center = (_min + _max).scale_result(0.5f);
	center = transform.transform_point(center);
	return point - center;
}

void st_oobb::get_corners(std::vector<st_vec3f>& corners) const
{
	st_vec3f x_hvec = _half_vectors[0];
	st_vec3f y_hvec = _half_vectors[1];
	st_vec3f z_hvec = _half_vectors[2];

	corners.push_back(_center - x_hvec - y_hvec - z_hvec);
	corners.push_back(_center - x_hvec - y_hvec + z_hvec);
	corners.push_back(_center - x_hvec + y_hvec - z_hvec);
	corners.push_back(_center - x_hvec + y_hvec + z_hvec);
	corners.push_back(_center + x_hvec - y_hvec - z_hvec);
	corners.push_back(_center + x_hvec - y_hvec + z_hvec);
	corners.push_back(_center + x_hvec + y_hvec - z_hvec);
	corners.push_back(_center + x_hvec + y_hvec + z_hvec);
}

void st_oobb::get_debug_draw(const st_mat4f& transform, st_dynamic_drawcall* drawcall)
{
	get_corners(drawcall->_positions);
	drawcall->_positions.push_back(st_vec3f::zero_vector());
	drawcall->_positions.push_back(_half_vectors[0]);
	drawcall->_positions.push_back(_half_vectors[1]);
	drawcall->_positions.push_back(_half_vectors[2]);

	uint32_t indices[] =
	{
		0, 1,
		1, 3,
		3, 2,
		2, 0,
		0, 4,
		4, 6,
		6, 2,
		4, 5,
		5, 7,
		7, 6,
		3, 7,
		1, 5,
		8, 9,
		8, 10,
		8, 11,
	};

	for (int i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i)
	{
		drawcall->_indices.push_back(indices[i]);
	}

	drawcall->_color = { 0.0f, 1.0f, 0.0f };
	drawcall->_draw_mode = GL_LINES;
	drawcall->_transform = transform;
	drawcall->_material = nullptr;
}

void st_oobb::get_inertia_tensor(st_mat4f& tensor, float mass)
{
	float one_over_twelve = 1.0f / 12.0f;
	float width2 = st_powf(_half_vectors[0].mag() * 2, 2);
	float height2 = st_powf(_half_vectors[1].mag() * 2, 2);
	float depth2 = st_powf(_half_vectors[2].mag() * 2, 2);

	tensor.make_identity();
	tensor.data[0][0] = one_over_twelve * mass * (height2 + depth2);
	tensor.data[1][1] = one_over_twelve * mass * (width2 + depth2);
	tensor.data[2][2] = one_over_twelve * mass * (width2 + height2);
}

st_vec3f st_oobb::get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const
{
	st_vec3f center = transform.transform_point(_center);
	return point - center;
}

void st_convex_hull::get_debug_draw(const st_mat4f& transform, st_dynamic_drawcall* drawcall)
{
	// TODO
}

void st_convex_hull::get_inertia_tensor(st_mat4f& tensor, float mass)
{
	// Unimplemented.
}

st_vec3f st_convex_hull::get_offset_to_point(const st_mat4f& transform, const st_vec3f& point) const
{
	// Unimplemented.
	return st_vec3f::zero_vector();
}
