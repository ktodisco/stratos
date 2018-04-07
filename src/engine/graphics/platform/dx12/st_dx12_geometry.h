#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cstdint>
#include <vector>

#include <wrl.h>

class st_vertex_format;

class st_dx12_geometry
{
public:

	st_dx12_geometry(
		const st_vertex_format* format,
		void* vertex_data,
		uint32_t vertex_size,
		uint32_t vertex_count,
		uint16_t* index_data,
		uint32_t index_count);
	~st_dx12_geometry();

	void draw(struct st_static_drawcall& draw_call);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> _vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW _vertex_buffer_view;
	Microsoft::WRL::ComPtr<ID3D12Resource> _index_buffer;
	D3D12_INDEX_BUFFER_VIEW _index_buffer_view;
	uint32_t _index_count = 0;
};

#endif
