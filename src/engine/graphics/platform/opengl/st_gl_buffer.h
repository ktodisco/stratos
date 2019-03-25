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

class st_gl_buffer
{
public:
	st_gl_buffer(const uint32_t count, const size_t element_size);
	~st_gl_buffer();

	void update(const class st_gl_render_context* context, void* data, const uint32_t count);
	void set_meta(std::string name) { _name = name; }

	uint32_t get_count() const { return _count; }
	size_t get_element_size() const { return _element_size; }

private:
	GLuint _buffer;
	uint32_t _count;
	size_t _element_size;
	std::string _name;
};

#endif
