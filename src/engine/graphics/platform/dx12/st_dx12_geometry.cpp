/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_geometry.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_buffer.h>
#include <graphics/platform/dx12/st_dx12_render_context.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/geometry/st_vertex_format.h>
#include <graphics/st_drawcall.h>

#include <cassert>

st_dx12_geometry::st_dx12_geometry(
	const st_vertex_format* format,
	void* vertex_data,
	uint32_t vertex_size,
	uint32_t vertex_count,
	uint16_t* index_data,
	uint32_t index_count)
{
	// Create the vertex buffer resource.
	_vertex_buffer = std::make_unique<st_dx12_buffer>(vertex_count, vertex_size, e_st_buffer_usage::vertex);
	_vertex_buffer->update(st_dx12_render_context::get(), vertex_data, 0, vertex_count);

	// Create the index buffer resource.
	_index_count = index_count;

	_index_buffer = std::make_unique<st_dx12_buffer>(index_count, sizeof(uint16_t), e_st_buffer_usage::index);
	_index_buffer->update(st_dx12_render_context::get(), index_data, 0, index_count);
}

st_dx12_geometry::~st_dx12_geometry()
{
}

void st_dx12_geometry::draw(st_static_drawcall& draw_call)
{
	draw_call._vertex_buffer = _vertex_buffer.get();
	draw_call._index_buffer = _index_buffer.get();
	draw_call._index_count = _index_count;
	draw_call._draw_mode = st_primitive_topology_triangles;
}

#endif
