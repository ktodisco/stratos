#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <cstdint>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

/*
** Represents a shader uniform (constant).
** @see st_shader
*/
class st_uniform
{
	friend class st_program;

public:
	void set(const struct st_vec3f& vec);
	void set(const struct st_mat4f& mat);
	void set(const struct st_mat4f* mats, uint32_t count);
	void set(const class st_texture& tex, uint32_t unit);

private:
	st_uniform(int32_t location);

	const int32_t _location;
};

/*
** Represents a shader.
** @see st_program
*/
class st_shader
{
	friend class st_program;

public:
	st_shader(const char* source, GLenum type);
	~st_shader();

	bool compile();

	std::string get_compile_log() const;

private:
	uint32_t _handle;
	const char* _source;
};

/*
** Represents a full shading pipeline.
** Vertex shader and fragment shader linked together.
** @see st_shader
*/
class st_program
{
public:
	st_program();
	~st_program();

	void attach(const st_shader& shader);
	void detach(const st_shader& shader);

	bool link();

	std::string get_link_log() const;

	st_uniform get_uniform(const char* name);

	void use();

private:
	uint32_t _handle;
};
