#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <math/st_vec4f.h>

#include <cstdint>
#include <memory>

class st_render_texture
{
public:

	st_render_texture(
		class st_graphics_context* context,
		uint32_t width,
		uint32_t height,
		e_st_format format,
		e_st_texture_usage_flags usage,
		e_st_texture_state initial_state,
		st_clear_value clear,
		const char* name);
	~st_render_texture();

	st_texture* get_texture() { return _texture.get(); }
	st_texture_view* get_target_view() const { return _rtv.get(); }
	st_texture_view* get_resource_view() const { return _srv.get(); }
	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }
	e_st_format get_format() const { return _format; }
	st_clear_value get_clear_value() const { return _clear; }

private:
	// TODO: Better to store a desc, or pull from the underlying texture.
	uint32_t _width;
	uint32_t _height;
	e_st_format _format;
	st_clear_value _clear;

	std::unique_ptr<struct st_texture> _texture;
	std::unique_ptr<struct st_texture_view> _rtv;
	std::unique_ptr<struct st_texture_view> _srv;
};
