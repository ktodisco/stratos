/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_shader.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/opengl/st_gl_texture.h>

#include <math/st_mat4f.h>
#include <math/st_vec2f.h>
#include <math/st_vec3f.h>
#include <math/st_vec4f.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

void load_shader(const std::string& filename, std::string& contents)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	assert(file.is_open());

	contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

st_gl_uniform::st_gl_uniform(int32_t location) : _location(location) {}

void st_gl_uniform::set(const st_vec2f& vec)
{
	glUniform2fv(_location, 1, vec.axes);
}

void st_gl_uniform::set(const st_vec3f& vec)
{
	glUniform3fv(_location, 1, vec.axes);
}

void st_gl_uniform::set(const struct st_vec4f& vec)
{
	glUniform4fv(_location, 1, vec.axes);
}

void st_gl_uniform::set(const st_mat4f& mat)
{
	glUniformMatrix4fv(_location, 1, GL_FALSE, (const GLfloat*)mat.data);
}

void st_gl_uniform::set(const st_mat4f* mats, uint32_t count)
{
	glUniformMatrix4fv(_location, count, GL_FALSE, (const GLfloat*)mats[0].data);
}

void st_gl_uniform::set(const st_gl_texture& tex, uint32_t unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, (GLuint)tex.get_handle());
	glUniform1i(_location, unit);
}

st_gl_shader_component::st_gl_shader_component(const char* source, uint32_t type)
{
	_handle = glCreateShader(type);
	glShaderSource(_handle, 1, &source, 0);
}

st_gl_shader_component::~st_gl_shader_component()
{
	glDeleteShader(_handle);
}

bool st_gl_shader_component::compile()
{
	glCompileShader(_handle);

	int32_t compile_status = GL_FALSE;
	glGetShaderiv(_handle, GL_COMPILE_STATUS, &compile_status);
	return compile_status == GL_TRUE;
}

std::string st_gl_shader_component::get_compile_log() const
{
	int32_t length;
	glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &length);

	std::string log(length, '\0');
	glGetShaderInfoLog(_handle, length, &length, &log[0]);
	return log;
}

st_gl_shader::st_gl_shader(const char* source, uint8_t type)
{
	_handle = glCreateProgram();

	// TODO: Handle non-vertex and non-pixel types.
	std::string source_vs;
	load_shader(std::string(source) + std::string("_vert.glsl"), source_vs);

	std::string source_fs;
	load_shader(std::string(source) + std::string("_frag.glsl"), source_fs);

	_vs = new st_gl_shader_component(source_vs.c_str(), GL_VERTEX_SHADER);
	if (!_vs->compile())
	{
		std::cerr << "Failed to compile vertex shader:" << std::endl << _vs->get_compile_log() << std::endl;
	}

	_fs = new st_gl_shader_component(source_fs.c_str(), GL_FRAGMENT_SHADER);
	if (!_fs->compile())
	{
		std::cerr << "Failed to compile fragment shader:\n\t" << std::endl << _fs->get_compile_log() << std::endl;
	}

	attach(*_vs);
	attach(*_fs);
	if (!link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << get_link_log() << std::endl;
	}
}

st_gl_shader::~st_gl_shader()
{
	glDeleteProgram(_handle);
}

void st_gl_shader::attach(const st_gl_shader_component& shader)
{
	glAttachShader(_handle, shader._handle);
}

void st_gl_shader::detach(const st_gl_shader_component& shader)
{
	glDetachShader(_handle, shader._handle);
}

bool st_gl_shader::link()
{
	glLinkProgram(_handle);

	int32_t link_status = GL_FALSE;
	glGetProgramiv(_handle, GL_LINK_STATUS, &link_status);
	return link_status == GL_TRUE;
}

std::string st_gl_shader::get_link_log() const
{
	int32_t length;
	glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &length);

	std::string log(length, '\0');
	glGetProgramInfoLog(_handle, length, &length, &log[0]);
	return log;
}

st_gl_uniform st_gl_shader::get_uniform(const char* name) const
{
	int32_t location = glGetUniformLocation(_handle, name);
	return st_gl_uniform(location);
}

void st_gl_shader::use() const
{
	glUseProgram(_handle);
}

#endif
