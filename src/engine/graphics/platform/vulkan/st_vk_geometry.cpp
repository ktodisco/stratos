/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_geometry.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>
#include <graphics/st_drawcall.h>

st_vk_geometry::st_vk_geometry(
	const st_vertex_format* format,
	void* vertex_data,
	uint32_t vertex_size,
	uint32_t vertex_count,
	uint16_t* index_data,
	uint32_t index_count)
	: _index_count(index_count)
{
	st_vk_render_context* context = st_vk_render_context::get();

	context->create_buffer(
		vertex_size * vertex_count,
		e_st_buffer_usage::vertex | e_st_buffer_usage::transfer_dest,
		_vertex_buffer,
		_vertex_memory);
	context->create_buffer(
		index_count * sizeof(uint16_t),
		e_st_buffer_usage::index | e_st_buffer_usage::transfer_dest,
		_index_buffer,
		_index_memory);

	context->update_buffer(_vertex_buffer, 0, vertex_size * vertex_count, vertex_data);
	context->update_buffer(_index_buffer, 0, index_count * sizeof(uint16_t), index_data);
}

st_vk_geometry::~st_vk_geometry()
{
	st_vk_render_context* context = st_vk_render_context::get();

	context->destroy_buffer(_vertex_buffer, _vertex_memory);
	context->destroy_buffer(_index_buffer, _index_memory);
}

void st_vk_geometry::draw(st_static_drawcall& draw_call)
{
	draw_call._vertex_buffer = &_vertex_buffer;
	draw_call._index_buffer = &_index_buffer;
	draw_call._index_count = _index_count;
	draw_call._draw_mode = st_primitive_topology_triangles;
}

#endif
