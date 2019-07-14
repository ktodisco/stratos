#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <string>

class st_vk_constant_buffer
{
public:
	st_vk_constant_buffer(const size_t size) {}
	~st_vk_constant_buffer() {}

	void add_constant(
		const std::string& name,
		const e_st_shader_constant_type constant_type) {}

	void update(const class st_vk_render_context* context, void* data) {}

	//D3D12_GPU_VIRTUAL_ADDRESS get_virtual_address() const { return _constant_buffer->GetGPUVirtualAddress(); }
	size_t get_size() const { return _size; }

private:
	size_t _size;
	//Microsoft::WRL::ComPtr<ID3D12Resource> _constant_buffer;
	uint8_t* _constant_buffer_head = nullptr;
};

#endif
