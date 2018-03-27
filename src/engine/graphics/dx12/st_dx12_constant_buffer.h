#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <string>
#include <Windows.h>
#include <wrl.h>

class st_dx12_constant_buffer
{
public:
	st_dx12_constant_buffer(const size_t size);
	~st_dx12_constant_buffer();

	void add_constant(
		const std::string& name,
		const e_st_shader_constant_type constant_type);

	void update(const class st_dx12_render_context* context, void* data);

	void commit(class st_dx12_render_context* context);

private:
	size_t _size;
	Microsoft::WRL::ComPtr<ID3D12Resource> _constant_buffer;
	// TODO: The constant buffer view offset, as well as other shader resource view
	// offsets, should be managed by an independent class which can group commonly
	// accessed resources and create contiguous views for them.
	uint32_t _constant_buffer_offset = 0;
	uint8_t* _constant_buffer_head = nullptr;
};

#endif
