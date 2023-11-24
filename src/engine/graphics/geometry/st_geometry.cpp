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

	_vertex_buffer = st_render_context::get()->create_buffer(
		vertex_count,
		vertex_size,
		e_st_buffer_usage::vertex);

	// TODO.
	// Map the buffer to a CPU write address and copy the data.
	//uint8_t* buffer_begin;
	//D3D12_RANGE range = { 0, 0 };
	//HRESULT result = _vertex_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	//
	//if (result != S_OK)
	//{
	//	assert(false);
	//}
	//
	//memcpy(buffer_begin, vertex_data, vertex_buffer_size);
	//_vertex_buffer->Unmap(0, nullptr);
	//
	//_vertex_buffer_view.BufferLocation = _vertex_buffer->GetGPUVirtualAddress();
	//_vertex_buffer_view.StrideInBytes = vertex_size;
	//_vertex_buffer_view.SizeInBytes = vertex_buffer_size;
	//
	//// Create the index buffer resource.
	//_index_count = index_count;
	//const uint32_t index_buffer_size = _index_count * sizeof(uint16_t);
	//
	//st_dx12_render_context::get()->create_buffer(index_buffer_size, _index_buffer.GetAddressOf());
	//
	//// Map the buffer to a CPU write address and copy the data.
	//result = _index_buffer->Map(0, &range, reinterpret_cast<void**>(&buffer_begin));
	//
	//if (result != S_OK)
	//{
	//	assert(false);
	//}
	//
	//memcpy(buffer_begin, index_data, index_buffer_size);
	//_index_buffer->Unmap(0, nullptr);
	//
	//_index_buffer_view.BufferLocation = _index_buffer->GetGPUVirtualAddress();
	//_index_buffer_view.Format = DXGI_FORMAT_R16_UINT;
	//_index_buffer_view.SizeInBytes = index_buffer_size;
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
