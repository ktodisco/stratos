#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/geometry/st_vertex_format.h>

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
	st_vec4f _color = st_vec4f::zero_vector();
	st_vec2f _uv = st_vec2f::zero_vector();

	uint32_t _joints[4] = { 0, 0, 0, 0 };
	float _weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct st_model_data
{
	st_model_data();
	~st_model_data();

	st_vertex_format _vertex_format;

	std::vector<st_vertex> _vertices;
	std::vector<uint16_t> _indices;
	const char* _texture_name;

	struct st_skeleton* _skeleton = 0;
};
