#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <climits>
#include <cstdint>
#include <vector>

#include "math/st_mat4f.h"
#include "math/st_vec2f.h"
#include "math/st_vec3f.h"

struct st_vertex
{
	static const uint32_t k_max_joint_weights = 4;

	st_vec3f _position = st_vec3f::zero_vector();
	st_vec3f _normal = st_vec3f::zero_vector();
	st_vec3f _color = st_vec3f::zero_vector();
	st_vec2f _uv = st_vec2f::zero_vector();

	uint32_t _joints[4] = { 0, 0, 0, 0 };
	float _weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

enum st_vertex_attribute
{
	k_vertex_attribute_normal = 1,
	k_vertex_attribute_color = 2,
	k_vertex_attribute_uv = 4,
	k_vertex_attribute_weight = 8,
};

struct st_model
{
	st_model();
	~st_model();

	uint32_t _vertex_format = 0;

	std::vector<st_vertex> _vertices;
	std::vector<uint16_t> _indices;
	const char* _texture_name;

	struct st_skeleton* _skeleton = 0;
};
