#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

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

struct st_gl_constant
{
	std::string _name;
	e_st_shader_constant_type _type;
};

struct st_gl_buffer : public st_buffer
{
	GLuint _buffer;
	uint32_t _count;
	size_t _element_size;
	e_st_buffer_usage_flags _usage;

	//union
	//{
		uint8_t* _storage;
		std::vector<st_gl_constant> _constants;
	//};

	std::string _name;
};

struct st_gl_buffer_view : public st_buffer_view
{
	union
	{
		st_gl_buffer* _buffer;
		uint32_t _vao;
	};
};

struct st_gl_geometry : public st_geometry
{
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;
};

struct st_gl_pipeline : public st_pipeline
{
	struct st_pipeline_state_desc _state_desc;
};

struct st_gl_render_pass : public st_render_pass
{
	std::unique_ptr<class st_gl_framebuffer> _framebuffer;
	st_viewport _viewport;
};

struct st_gl_resource_table : public st_resource_table
{
	std::vector<struct st_texture*> _srvs;
	std::vector<GLuint> _samplers;
};

struct st_gl_texture : public st_texture
{
	uint32_t _handle;
	uint32_t _width;
	uint32_t _height;
	uint32_t _levels = 1;
	e_st_format _format;
	std::string _name;
};

struct st_gl_texture_view : public st_texture_view
{
	const st_gl_texture* _texture;
};

struct st_gl_vertex_format : public st_vertex_format
{
	std::vector<struct st_vertex_attribute> _attributes;
};
