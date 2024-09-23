/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_framebuffer.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/st_render_texture.h>

#include <cassert>
#include <vector>

st_gl_framebuffer::st_gl_framebuffer(
	uint32_t count,
	st_render_texture** targets,
	st_render_texture* depth_stencil) :
	_target_count(count)
{
	glGenFramebuffers(1, &_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

	uint32_t color_attachment = GL_COLOR_ATTACHMENT0;
	st_render_texture** target = targets;
	for (uint32_t color_target = 0; color_target < count; ++color_target)
	{
		st_gl_texture* texture = static_cast<st_gl_texture*>((*target)->get_texture());
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			color_attachment,
			GL_TEXTURE_2D,
			texture->_handle,
			0);

		target++;
		color_attachment++;
	}

	if (depth_stencil)
	{
		st_gl_texture* ds = static_cast<st_gl_texture*>(depth_stencil->get_texture());
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D,
			ds->_handle,
			0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		const GLubyte* error_string = gluErrorString(status);
		assert(false);
		GLenum test = GL_INVALID_OPERATION;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

st_gl_framebuffer::~st_gl_framebuffer()
{
	glDeleteFramebuffers(1, &_framebuffer);
}

void st_gl_framebuffer::bind(class st_graphics_context* context)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

	GLenum targets[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7,
	};

	glDrawBuffers(_target_count, targets);
}

void st_gl_framebuffer::unbind(class st_graphics_context* context)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum target[] = { GL_BACK_LEFT };
	glDrawBuffers(1, target);
}

#endif
