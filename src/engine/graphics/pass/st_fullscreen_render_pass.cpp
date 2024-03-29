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
#if defined(ST_GRAPHICS_API_VULKAN)
		-1.0f, 1.0f, 0.0f,
		3.0f, 1.0f, 0.0f,
		-1.0f, -3.0f, 0.0f,
#else
		-1.0f, -1.0f, 0.0f,
		3.0f, -1.0f, 0.0f,
		-1.0f, 3.0f, 0.0f,
#endif
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
}
