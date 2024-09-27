#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <cstdint>

class st_gl_framebuffer
{
public:
	st_gl_framebuffer(
		uint32_t count,
		struct st_target_desc* targets,
		struct st_target_desc* depth_stencil);
	~st_gl_framebuffer();

	void bind(class st_graphics_context* context);
	void unbind(class st_graphics_context* context);

	GLuint get_handle() { return _framebuffer; };

private:
	uint32_t _target_count;
	GLuint _framebuffer;
};

#endif
