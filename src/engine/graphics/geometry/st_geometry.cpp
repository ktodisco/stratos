/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/geometry/st_geometry.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/st_drawcall.h>

#include <cassert>

st_geometry::st_geometry(
	const st_vertex_format* format,
	void* vertex_data,
	uint32_t vertex_size,
	uint32_t vertex_count,
	uint16_t* index_data,
	uint32_t index_count)
{
	// Create the vertex buffer resource.
	const uint32_t vertex_buffer_size = vertex_count * vertex_size;

	st_render_context* context = st_render_context::get();

	_vertex_buffer = context->create_buffer(
		vertex_count,
		vertex_size,
		e_st_buffer_usage::vertex);

	// Map the buffer to a CPU write address and copy the data.
	uint8_t* buffer_begin;
	st_range range = { 0, 0 };
	context->map(_vertex_buffer.get(), 0, range, reinterpret_cast<void**>(&buffer_begin));
	
	memcpy(buffer_begin, vertex_data, vertex_buffer_size);
	context->unmap(_vertex_buffer.get(), 0, range);
	
	_vertex_buffer_view = context->create_buffer_view(_vertex_buffer.get());
	
	// Create the index buffer resource.
	_index_count = index_count;
	const uint32_t index_buffer_size = _index_count * sizeof(uint16_t);
	
	_index_buffer = context->create_buffer(
		index_count,
		sizeof(uint16_t),
		e_st_buffer_usage::index);
	
	// Map the buffer to a CPU write address and copy the data.
	context->map(_index_buffer.get(), 0, range, reinterpret_cast<void**>(&buffer_begin));
	
	memcpy(buffer_begin, index_data, index_buffer_size);
	context->unmap(_index_buffer.get(), 0, range);
	
	_index_buffer_view = context->create_buffer_view(_index_buffer.get());
}

st_geometry::~st_geometry()
{
}

void st_geometry::draw(st_static_drawcall& draw_call)
{
	draw_call._vertex_buffer_view = _vertex_buffer_view.get();
	draw_call._index_buffer_view = _index_buffer_view.get();
	draw_call._index_count = _index_count;
	draw_call._draw_mode = st_primitive_topology_triangles;
}
