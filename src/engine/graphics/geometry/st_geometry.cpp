#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/geometry/st_geometry.h>

#include <graphics/st_drawcall.h>
#include <graphics/st_graphics_context.h>

st_geometry::st_geometry(
	const struct st_vertex_format* format,
	void* vertex_data,
	uint32_t vertex_size,
	uint32_t vertex_count,
	uint16_t* index_data,
	uint32_t index_count)
{
	// Create the vertex buffer resource.
	const uint32_t vertex_buffer_size = vertex_count * vertex_size;

	st_graphics_context* context = st_graphics_context::get();

	_vertex_buffer = context->create_buffer(
		vertex_count,
		vertex_size,
		e_st_buffer_usage::vertex | e_st_buffer_usage::transfer_dest);

	uint8_t* head;
	context->map(_vertex_buffer.get(), 0, { 0, 0 }, (void**)&head);
	memcpy(head, vertex_data, vertex_count * vertex_size);
	context->unmap(_vertex_buffer.get(), 0, { 0, 0 });

	// Create the index buffer resource.
	_index_count = index_count;
	const uint32_t index_buffer_size = index_count * sizeof(uint16_t);

	_index_buffer = context->create_buffer(
		index_count,
		sizeof(uint16_t),
		e_st_buffer_usage::index | e_st_buffer_usage::transfer_dest);

	context->map(_index_buffer.get(), 0, { 0, 0 }, (void**)&head);
	memcpy(head, index_data, index_count * sizeof(uint16_t));
	context->unmap(_index_buffer.get(), 0, { 0, 0 });
}

st_geometry::~st_geometry()
{
	_vertex_buffer = nullptr;
	_index_buffer = nullptr;
}

void st_geometry::draw(st_static_drawcall& draw_call)
{
	draw_call._vertex_buffer = _vertex_buffer.get();
	draw_call._index_buffer = _index_buffer.get();
	draw_call._index_count = _index_count;
}
