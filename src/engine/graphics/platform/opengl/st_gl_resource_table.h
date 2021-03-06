#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <vector>

class st_gl_resource_table
{
public:
	st_gl_resource_table();
	~st_gl_resource_table();

	void set_constant_buffers(uint32_t count, class st_constant_buffer** cbs);
	void set_textures(uint32_t count, class st_texture** textures);
	void set_buffers(uint32_t count, class st_buffer** buffers);

	void bind(class st_gl_render_context* context);

private:
	std::vector<st_texture*> _srvs;
	std::vector<GLuint> _samplers;
};

#endif
