#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <string>
#include <Windows.h>
#include <wrl.h>

class st_dx12_constant_buffer
{
public:
	st_dx12_constant_buffer(const size_t size);
	~st_dx12_constant_buffer();

	void add_constant(std::string name, const size_t size);

	void update(const class st_dx12_render_context* context, void* data);

	void commit(class st_dx12_render_context* context);

private:
	size_t _size;
	Microsoft::WRL::ComPtr<ID3D12Resource> _constant_buffer;
	uint32_t _constant_buffer_offset = 0;
	uint8_t* _constant_buffer_head = nullptr;
};
