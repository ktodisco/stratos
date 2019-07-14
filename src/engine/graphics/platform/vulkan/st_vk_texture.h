#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

class st_vk_texture
{
public:
	st_vk_texture() {}
	st_vk_texture(uint32_t width, uint32_t height) {}
	~st_vk_texture() {}

	void load_from_data(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		void* data) {}
	void set_meta(const char* name) {}

	void bind(class st_dx12_render_context* context) {}

	void set_name(std::string name) {}

	//ID3D12Resource* get_resource() const { return _handle.Get(); }
	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }
	uint32_t get_levels() const { return _levels; }
	e_st_format get_format() const { return _format; }

protected:
	//Microsoft::WRL::ComPtr<ID3D12Resource> _handle;

	uint32_t _width;
	uint32_t _height;
	uint32_t _levels = 1;
	e_st_format _format;
	uint32_t _sampler;
	uint32_t _srv;

	friend class st_vk_render_texture;
};

#endif
