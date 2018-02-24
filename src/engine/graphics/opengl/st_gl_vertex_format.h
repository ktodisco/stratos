#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <cstdint>

class st_gl_vertex_format
{
public:
	st_gl_vertex_format() {}
	~st_gl_vertex_format() {}

	virtual uint32_t get_attribute_count() const = 0;
	virtual const class st_vertex_attribute& get_attribute(uint32_t index) const = 0;

	void platform_finalize() {}
};

#endif
