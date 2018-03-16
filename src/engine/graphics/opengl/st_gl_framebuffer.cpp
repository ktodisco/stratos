/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_framebuffer.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/st_render_texture.h>
#include <graphics/st_texture.h>

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
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			color_attachment,
			GL_TEXTURE_2D,
			(*target)->get_handle(),
			0);

		target++;
		color_attachment++;
	}

	if (depth_stencil)
	{
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D,
			depth_stencil->get_handle(),
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

void st_gl_framebuffer::bind(class st_render_context* context)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

	// TODO: There's got to be a more clever way to do this.
	std::vector<GLenum> targets;
	if (_target_count > 0) targets.push_back(GL_COLOR_ATTACHMENT0);
	if (_target_count > 1) targets.push_back(GL_COLOR_ATTACHMENT1);
	glDrawBuffers(_target_count, &targets[0]);
}

void st_gl_framebuffer::unbind(class st_render_context* context)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum target[] = { GL_BACK_LEFT };
	glDrawBuffers(1, target);
}

#endif
