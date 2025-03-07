#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#define GLEW_STATIC
#include <graphics/st_pipeline_state_desc.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/wglew.h>

#include <Windows.h>

#include <string>
#include <vector>

#define GLUEME(x, y) x##y
#define GLUE(x, y) GLUEME(x, y)
#define ST_GL_CALL(call) \
	call; \
	GLenum GLUE(er, __LINE__) = glGetError(); \
	if (GLUE(er, __LINE__) != GL_NO_ERROR) \
	{ \
		assert(false); \
	}

void gl_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

void get_pixel_format_and_type(
	e_st_format format,
	GLenum & pixel_format,
	GLenum & type);

struct st_gl_buffer : public st_buffer
{
	st_gl_buffer() {}
	~st_gl_buffer()
	{
		if (_storage)
			free(_storage);
		glDeleteBuffers(1, &_buffer);
	}

	GLuint _buffer;
	uint32_t _count;
	size_t _element_size;
	e_st_buffer_usage_flags _usage;
	
	uint8_t* _storage;
};

struct st_gl_buffer_view : public st_buffer_view
{
	const st_buffer_view_desc _desc;
};

struct st_gl_pipeline : public st_pipeline
{
	st_gl_pipeline() {}

	union
	{
		struct st_graphics_state_desc _graphics_desc;
		struct st_compute_state_desc _compute_desc;
	};
};

struct st_gl_render_pass : public st_render_pass
{
	std::unique_ptr<class st_gl_framebuffer> _framebuffer;
	st_viewport _viewport;
};

struct st_gl_resource_table : public st_resource_table
{
	~st_gl_resource_table()
	{
		_srvs.clear();
		_samplers.clear();
	}

	std::vector<const struct st_buffer_view*> _constant_buffers;
	std::vector<const struct st_texture_view*> _srvs;
	std::vector<const struct st_sampler*> _samplers;
	std::vector<const struct st_buffer_view*> _buffers;
	std::vector<const struct st_texture_view*> _uavs;
};

struct st_gl_sampler : public st_sampler
{
	~st_gl_sampler() { glDeleteSamplers(1, &_handle); }

	uint32_t _handle;
};

struct st_gl_texture : public st_texture
{
	~st_gl_texture() { glDeleteTextures(1, &_handle); }

	uint32_t _handle;
	uint32_t _width;
	uint32_t _height;
	uint32_t _levels = 1;
	e_st_format _format;
};

struct st_gl_texture_view : public st_texture_view
{
	const st_texture_view_desc _desc;
};

struct st_gl_vertex_format : public st_vertex_format
{
	std::vector<struct st_vertex_attribute> _attributes;
};
