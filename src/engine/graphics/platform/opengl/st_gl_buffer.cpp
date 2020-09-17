/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_buffer.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_render_context.h>
#include <graphics/platform/opengl/st_gl_shader.h>

st_gl_buffer::st_gl_buffer(const uint32_t count, const size_t element_size, const e_st_buffer_usage_flags usage)
	: _count(count), _element_size(element_size)
{
	glGenBuffers(1, &_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, count * element_size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

st_gl_buffer::~st_gl_buffer()
{
	glDeleteBuffers(1, &_buffer);
}

void st_gl_buffer::update(const class st_gl_render_context* context, void* data, const uint32_t count)
{
	const st_gl_shader* shader = context->get_bound_shader();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _buffer);

	st_gl_shader_storage_block ssb = shader->get_shader_storage_block(_name.c_str());
	ssb.set(_buffer, data, count * _element_size);
}

#endif
