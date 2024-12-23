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
