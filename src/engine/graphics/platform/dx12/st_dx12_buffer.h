#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cstdint>

class st_dx12_buffer
{
public:
	st_dx12_buffer(const uint32_t count, const size_t element_size);
	~st_dx12_buffer();

	void update(const class st_dx12_render_context* context, void* data, const uint32_t count);

	ID3D12Resource* get_resource() const { return _buffer.Get(); }
	uint32_t get_count() const { return _count; }
	size_t get_element_size() const { return _element_size; }

private:
	uint32_t _count;
	size_t _element_size;
	Microsoft::WRL::ComPtr<ID3D12Resource> _buffer;
	uint8_t* _buffer_head = nullptr;
};

#endif
