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
	st_vk_constant_buffer(const size_t size);
	~st_vk_constant_buffer();

	void add_constant(
		const std::string& name,
		const e_st_shader_constant_type constant_type) {}

	void update(const class st_vk_render_context* context, void* data);
	size_t get_size() const { return _size; }
	const vk::Buffer& get_resource() const { return _buffer; } 

private:
	vk::Buffer _buffer;
	size_t _size;
};

#endif
