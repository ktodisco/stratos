#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <cstdint>

enum e_st_vertex_attribute
{
	st_vertex_attribute_position,
	st_vertex_attribute_normal,
	st_vertex_attribute_binormal,
	st_vertex_attribute_tangent,
	st_vertex_attribute_color,
	st_vertex_attribute_uv,
	st_vertex_attribute_joints,
	st_vertex_attribute_weights
};

struct st_vertex_attribute
{
	st_vertex_attribute() = delete;
	st_vertex_attribute(e_st_vertex_attribute type, e_st_format format, uint32_t unit) :
		_type(type), _format(format), _unit(unit) {}
	~st_vertex_attribute() {}

	e_st_vertex_attribute _type;
	e_st_format _format;
	uint32_t _unit;
};

inline size_t calculate_vertex_size(const struct st_vertex_attribute* attributes, uint32_t count)
{
	size_t vertex_size = 0;

	for (uint32_t itr = 0; itr < count; ++itr)
	{
		const st_vertex_attribute* attr = &attributes[itr];
		vertex_size += bits_per_pixel(attr->_format) / 8;
	}

	return vertex_size;
}
