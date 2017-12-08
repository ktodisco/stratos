/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_program.h"

#include "st_texture.h"

#include "math/st_mat4f.h"
#include "math/st_vec3f.h"

#include <cassert>

#define GLEW_STATIC
#include <GL/glew.h>

void st_uniform::set(const st_vec3f& vec)
{
	glUniform3fv(_location, 1, vec.axes);
}

void st_uniform::set(const st_mat4f& mat)
{
	glUniformMatrix4fv(_location, 1, GL_TRUE, (const GLfloat*)mat.data);
}

void st_uniform::set(const st_mat4f* mats, uint32_t count)
{
	glUniformMatrix4fv(_location, count, GL_TRUE, (const GLfloat*)mats[0].data);
}

void st_uniform::set(const st_texture& tex, uint32_t unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tex._handle);
	glUniform1i(_location, unit);
}

st_uniform::st_uniform(int32_t location) : _location(location) {}

st_shader::st_shader(const char* source, GLenum type)
{
	_handle = glCreateShader(type);
	glShaderSource(_handle, 1, &source, 0);
}

st_shader::~st_shader()
{
	glDeleteShader(_handle);
}

bool st_shader::compile()
{
	glCompileShader(_handle);

	int32_t compile_status = GL_FALSE;
	glGetShaderiv(_handle, GL_COMPILE_STATUS, &compile_status);
	return compile_status == GL_TRUE;
}

std::string st_shader::get_compile_log() const
{
	int32_t length;
	glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &length);

	std::string log(length, '\0');
	glGetShaderInfoLog(_handle, length, &length, &log[0]);
	return log;
}

st_program::st_program()
{
	_handle = glCreateProgram();
}

st_program::~st_program()
{
	glDeleteProgram(_handle);
}

void st_program::attach(const st_shader& shader)
{
	glAttachShader(_handle, shader._handle);
}

void st_program::detach(const st_shader& shader)
{
	glDetachShader(_handle, shader._handle);
}

bool st_program::link()
{
	glLinkProgram(_handle);

	int32_t link_status = GL_FALSE;
	glGetProgramiv(_handle, GL_LINK_STATUS, &link_status);
	return link_status == GL_TRUE;
}

std::string st_program::get_link_log() const
{
	int32_t length;
	glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &length);

	std::string log(length, '\0');
	glGetProgramInfoLog(_handle, length, &length, &log[0]);
	return log;
}

st_uniform st_program::get_uniform(const char* name)
{
	int32_t location = glGetUniformLocation(_handle, name);
	return st_uniform(location);
}

void st_program::use()
{
	glUseProgram(_handle);
}
