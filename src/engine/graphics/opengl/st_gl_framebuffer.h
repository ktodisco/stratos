#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <cstdint>

class st_gl_framebuffer
{
public:
	st_gl_framebuffer(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil);
	~st_gl_framebuffer();

	void bind(class st_render_context* context);
	void unbind(class st_render_context* context);

private:
	GLuint _framebuffer;
};

#endif
