#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <cstdint>
#include <string>

class st_gl_texture
{
public:
	st_gl_texture();
	st_gl_texture(uint32_t width, uint32_t height);
	~st_gl_texture();

	void reserve_data(uint32_t width, uint32_t height, e_st_format format);
	void load_from_data(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		void* data);

	// TODO: This meta information is not ideal for textures that are shared between different
	// passes.  One solution is to enforce it to be named on-the-fly.  Another solution is to
	// createa texture view class that acts as this information on an individualized basis.
	void set_meta(const char* name);

	void set_name(std::string name);

	uint32_t get_handle() const { return _handle; }
	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }
	uint32_t get_levels() const { return _levels; }
	e_st_format get_format() const { return _format; }

	void bind(class st_gl_render_context* context, GLuint sampler);

protected:
	uint32_t _handle;
	uint32_t _width;
	uint32_t _height;
	uint32_t _levels = 1;
	e_st_format _format;
	std::string _name;

private:
	void get_pixel_format_and_type(
		e_st_format format,
		GLenum& pixel_format,
		GLenum& type);
};

#endif
