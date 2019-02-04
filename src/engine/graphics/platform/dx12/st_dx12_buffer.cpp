/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_buffer.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_render_context.h>

st_dx12_buffer::st_dx12_buffer(const uint32_t count, const size_t element_size) :
	_count(count),
	_element_size(element_size)
{
	// Structured buffers in DX12 are recommended to be 128-byte aligned.
	const uint32_t k_min_buffer_size = 128;

	// We store the original size so that the memcpy in update does not overread
	// the bounds of the passed memory.
	st_dx12_render_context::get()->create_buffer(align_value(_count * _element_size, k_min_buffer_size), _buffer.GetAddressOf());

	D3D12_RANGE range = {};
	_buffer->Map(0, &range, reinterpret_cast<void**>(&_buffer_head));
}

st_dx12_buffer::~st_dx12_buffer()
{
	_buffer->Unmap(0, nullptr);
}

void st_dx12_buffer::update(const st_dx12_render_context* context, void* data, const uint32_t count)
{
	memcpy(_buffer_head, data, count * _element_size);
}

#endif