/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_constant_buffer.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_render_context.h>

st_dx12_constant_buffer::st_dx12_constant_buffer(const size_t size) :
	_size(size)
{
	// Constant buffers in DX12 must be 256-byte aligned.
	const uint32_t k_min_cb_size = 256;

	// We store the original size so that the memcpy in update does not overread
	// the bounds of the passed memory.
	st_dx12_render_context::get()->create_buffer(align_value(_size, k_min_cb_size), _constant_buffer.GetAddressOf());

	D3D12_RANGE range = {};
	_constant_buffer->Map(0, &range, reinterpret_cast<void**>(&_constant_buffer_head));
}

st_dx12_constant_buffer::~st_dx12_constant_buffer()
{
	_constant_buffer->Unmap(0, nullptr);
}

void st_dx12_constant_buffer::add_constant(
		const std::string& name,
		const e_st_shader_constant_type constant_type)
{
}

void st_dx12_constant_buffer::update(const st_dx12_render_context* context, void* data)
{
	memcpy(_constant_buffer_head, data, _size);
}

#endif
