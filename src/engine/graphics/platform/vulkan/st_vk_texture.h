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
	// TODO: Finally set up proper RAII. Create the texture and texture view in the constructor,
	// and then require load to be called separately to actually upload data to the texture.
	st_vk_texture() = delete;
	st_vk_texture(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		e_st_texture_usage_flags usage);
	~st_vk_texture();

	void load_from_data(void* data);
	void set_meta(const char* name) {}

	void bind(class st_dx12_render_context* context);

	void set_name(std::string name);

	const vk::Image& get_resource() const { return _handle; }
	const vk::ImageView& get_resource_view() const { return _view; }
	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }
	uint32_t get_levels() const { return _levels; }
	e_st_format get_format() const { return _format; }

protected:
	vk::Image _handle;
	vk::ImageView _view;

	uint32_t _width;
	uint32_t _height;
	uint32_t _levels = 1;
	e_st_format _format;
	e_st_texture_usage_flags _usage;

	friend class st_vk_render_texture;
};

#endif
