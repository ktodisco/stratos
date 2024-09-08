/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <graphics/st_graphics.h>
#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/material/st_material.h>

#include <cstdint>

st_fullscreen_render_pass::st_fullscreen_render_pass()
{
	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format = st_render_context::get()->create_vertex_format(attributes.data(), attributes.size());

	const float verts[] =
	{
		// TODO: Vulkan dynamic switch.
#if 0 // defined(ST_GRAPHICS_API_VULKAN)
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

	_fullscreen_quad = st_render_context::get()->create_geometry(
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
