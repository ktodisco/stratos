/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/geometry/st_vertex_format.h>

#include <cstdint>

st_fullscreen_render_pass::st_fullscreen_render_pass()
{
	_vertex_format = std::make_unique<st_vertex_format>();
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format->finalize();

	const float verts[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
	};

	const uint16_t indices[] =
	{
		2, 1, 0,
		3, 2, 0,
	};

	_fullscreen_quad = std::make_unique<st_geometry>(
		_vertex_format.get(),
		(void*)verts,
		static_cast<uint32_t>(sizeof(float) * 3),
		4,
		(uint16_t*)indices,
		6);
}

st_fullscreen_render_pass::~st_fullscreen_render_pass()
{
}
