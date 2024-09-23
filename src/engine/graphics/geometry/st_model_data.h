#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <climits>
#include <cstdint>
#include <memory>
#include <vector>

#include "math/st_mat4f.h"
#include "math/st_vec2f.h"
#include "math/st_vec3f.h"

struct st_vertex
{
	static const uint32_t k_max_joint_weights = 4;

	// TODO: These need to be in a specific order because we offset based on accumulated data size
	// of each element.
	st_vec3f _position = st_vec3f::zero_vector();
	st_vec3f _normal = st_vec3f::zero_vector();
	st_vec3f _tangent = st_vec3f::zero_vector();
	st_vec4f _color = st_vec4f::zero_vector();
	st_vec2f _uv = st_vec2f::zero_vector();

	// TODO: To re-enable these, more robust runtime handling of varying vertex types needs to be
	// implemented. I.e. support both static and animated verts as separate types.
	//uint32_t _joints[4] = { 0, 0, 0, 0 };
	//float _weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct st_model_data
{
	st_model_data();
	~st_model_data();

	std::unique_ptr<struct st_vertex_format> _vertex_format;

	std::vector<st_vertex> _vertices;
	std::vector<uint16_t> _indices;
	const char* _texture_name;

	struct st_skeleton* _skeleton = 0;
};
