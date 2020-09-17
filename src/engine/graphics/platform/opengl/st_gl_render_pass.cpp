/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_render_pass.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_framebuffer.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_texture.h>

st_gl_render_pass::st_gl_render_pass(
	uint32_t count,
	class st_render_texture** targets,
	class st_render_texture* depth_stencil)
{
	// Naively, create the viewport from the first target.
	if (count > 0)
	{
		st_gl_viewport b;

		_viewport =
		{
			0,
			0,
			float(targets[0]->get_width()),
			float(targets[0]->get_height()),
			0.0f,
			1.0f,
		};
	}
	_framebuffer = std::make_unique<st_gl_framebuffer>(
		count,
		targets,
		depth_stencil);
}

st_gl_render_pass::~st_gl_render_pass()
{
	_framebuffer = nullptr;
}

void st_gl_render_pass::begin(
	st_render_context* context,
	st_vec4f* clear_values,
	const uint8_t clear_count)
{
	context->set_viewport(_viewport);
	_framebuffer->bind(context);
}

void st_gl_render_pass::end(class st_render_context* context)
{
}

#endif
