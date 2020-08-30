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
	st_vk_texture() = delete;
	st_vk_texture(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		e_st_texture_usage_flags usage,
		e_st_texture_state initial_state,
		void* data);
	~st_vk_texture();

	void set_meta(const char* name) {}

	void bind(class st_dx12_render_context* context);

	void set_name(std::string name);
	void transition(class st_vk_render_context* context, e_st_texture_state new_state);

	const vk::Image& get_resource() const { return _handle; }
	const vk::ImageView& get_resource_view() const { return _view; }
	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }
	uint32_t get_levels() const { return _levels; }
	e_st_format get_format() const { return _format; }
	e_st_texture_state get_state() const { return _state; }

protected:
	vk::Image _handle;
	vk::ImageView _view;

	uint32_t _width = 0;
	uint32_t _height = 0;
	uint32_t _levels = 1;
	e_st_format _format = st_format_unknown;
	e_st_texture_usage_flags _usage = static_cast<e_st_texture_usage_flags>(0);
	e_st_texture_state _state = st_texture_state_common;

	friend class st_vk_render_texture;
};

#endif
