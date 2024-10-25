/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <graphics/st_graphics_context.h>
#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/material/st_material.h>

#include <cstdint>
#include <vector>

st_fullscreen_render_pass::st_fullscreen_render_pass()
{
	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32b32_float, 0));
	_vertex_format = st_graphics_context::get()->create_vertex_format(attributes.data(), attributes.size());

	float verts[] =
	{
		-1.0f, -1.0f, 0.0f,
		3.0f, -1.0f, 0.0f,
		-1.0f, 3.0f, 0.0f,
	};

	const uint16_t indices[] =
	{
		0, 1, 2
	};

	_fullscreen_quad = std::make_unique<st_geometry>(
		_vertex_format.get(),
		(void*)verts,
		static_cast<uint32_t>(sizeof(float) * 3),
		3,
		(uint16_t*)indices,
		3);
}

st_fullscreen_render_pass::~st_fullscreen_render_pass()
{
	_vertex_format = nullptr;
	_fullscreen_quad = nullptr;
	_material = nullptr;
	_pipeline = nullptr;
}
