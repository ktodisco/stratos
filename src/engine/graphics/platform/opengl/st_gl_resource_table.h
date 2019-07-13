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

	void add_constant_buffer(class st_gl_constant_buffer* cb);
	void add_texture_resource(class st_gl_texture* sr);
	void add_buffer_resource(class st_gl_buffer* br);

	void bind(class st_gl_render_context* context);

private:
	std::vector<st_gl_texture*> _srvs;
	std::vector<GLuint> _samplers;
};

#endif
