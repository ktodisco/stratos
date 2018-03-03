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
	~st_gl_texture();

	void reserve_data(uint32_t width, uint32_t height, e_st_texture_format format);
	void load_from_data(uint32_t width, uint32_t height, e_st_texture_format format, void* data);
	bool load_from_file(const char* path);
	void set_meta(const char* name, const uint32_t slot);

	uint32_t get_handle() const { return _handle; }

	void bind(class st_gl_render_context* context);

private:
	uint32_t _handle;
	uint32_t _slot = 0;
	std::string _name;
};

#endif
