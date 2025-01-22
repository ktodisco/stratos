/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_shader.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/st_shader_manager.h>

#include <math/st_mat4f.h>
#include <math/st_vec2f.h>
#include <math/st_vec3f.h>
#include <math/st_vec4f.h>

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#define ST_GL_LOG_REFLECTION 0

void load_shader(const std::string& filename, std::string& contents)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	assert(file.is_open());

	contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

st_gl_uniform::st_gl_uniform(int32_t location, uint32_t binding) : _location(location), _binding(binding) {}

void st_gl_uniform::set_srv(const st_gl_texture* tex, const st_gl_sampler* sampler) const
{
	glActiveTexture(GL_TEXTURE0 + _location);
	glBindTexture(GL_TEXTURE_2D, (GLuint)tex->_handle);
	glUniform1i(_location, _location);
	glBindSampler(_location, (GLuint)sampler->_handle);
}

void st_gl_uniform::set_uav(const st_gl_texture* tex) const
{
	glBindImageTexture(_location, (GLuint)tex->_handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
}

st_gl_uniform& st_gl_uniform::operator=(const st_gl_uniform& u)
{
	_location = u._location;
	_binding = u._binding;
	return *this;
}

st_gl_uniform_block::st_gl_uniform_block(int32_t location) : _location(location)
{
}

// TODO: I think this can actually be deferred to the buffer type.
// Just pass the location back from the shader and CPU map the buffer object then call memcpy.
void st_gl_uniform_block::set(uint32_t buffer, void* data, size_t size) const
{
	glBindBufferRange(GL_UNIFORM_BUFFER, _location, buffer, 0, size);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
}

st_gl_shader_storage_block::st_gl_shader_storage_block(int32_t location)
	: _location(location)
{
}

void st_gl_shader_storage_block::set(uint32_t buffer) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _location, buffer);
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

st_gl_shader::st_gl_shader(const char* name, e_st_shader_type_flags type)
{
	_handle = glCreateProgram();

	auto load_compile_attach = [&](st_gl_shader_component* s, const char* suffix, GLenum type)
	{
		std::string source;
		load_shader(std::string(name) + std::string(suffix), source);

		s = new st_gl_shader_component(source.c_str(), type);
		if (!s->compile())
		{
			std::cerr << "Failed to compile shader:" << std::endl << s->get_compile_log() << std::endl;
			assert(false);
		}

		attach(*s);
	};

	if (type & e_st_shader_type::vertex) load_compile_attach(_vs, "_vert.glsl", GL_VERTEX_SHADER);
	if (type & e_st_shader_type::pixel) load_compile_attach(_fs, "_frag.glsl", GL_FRAGMENT_SHADER);
	if (type & e_st_shader_type::compute) load_compile_attach(_cs, "_comp.glsl", GL_COMPUTE_SHADER);

	if (!link())
	{
		std::cerr << "Failed to link shader program:\n\t" << std::endl << get_link_log() << std::endl;
		assert(false);
	}

	reflect();
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

const st_gl_uniform& st_gl_shader::get_uniform(uint32_t index) const
{
	std::vector<st_gl_uniform>::const_iterator itr =
		std::find_if(_texture_binds.begin(), _texture_binds.end(), [&index](const st_gl_uniform& u)
	{
		return u.get_binding() == index;
	});

	assert(itr != _texture_binds.end());

	return *itr;
}

const st_gl_uniform_block& st_gl_shader::get_uniform_block(uint32_t index) const
{
	std::vector<st_gl_uniform_block>::const_iterator itr =
		std::find_if(_constant_binds.begin(), _constant_binds.end(), [&index](const st_gl_uniform_block& b)
	{
		return b.get_location() == index;
	});

	assert(itr != _constant_binds.end());

	return *itr;
}

const st_gl_shader_storage_block& st_gl_shader::get_shader_storage_block(uint32_t index) const
{
	std::vector<st_gl_shader_storage_block>::const_iterator itr =
		std::find_if(_buffer_binds.begin(), _buffer_binds.end(), [&index](const st_gl_shader_storage_block& b)
	{
		return b.get_location() == index;
	});

	assert(itr != _buffer_binds.end());

	return *itr;
}

void st_gl_shader::use() const
{
	glUseProgram(_handle);
}

void st_gl_shader::reflect()
{
	// Reflect the shader resources to obtain binding points.
#if ST_GL_LOG_REFLECTION
	printf("Reflecting %s:\n", source);
#endif

	GLint active_blocks;
	glGetProgramInterfaceiv(_handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &active_blocks);
	_constant_binds.reserve(active_blocks);

#if ST_GL_LOG_REFLECTION
	GLint longest_block;
	glGetProgramInterfaceiv(_handle, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &longest_block);
	std::string block_name;
	block_name.reserve(longest_block);
#endif

	for (uint32_t i = 0; i < active_blocks; ++i)
	{
		GLint binding;
		GLenum props[] = { GL_BUFFER_BINDING };
		glGetProgramResourceiv(_handle, GL_UNIFORM_BLOCK, i, 1, props, 1, NULL, &binding);

#if ST_GL_LOG_REFLECTION
		glGetProgramResourceName(_handle, GL_UNIFORM_BLOCK, i, longest_block, NULL, block_name.data());
		printf("\tUniform block %s at location %d\n", block_name.c_str(), binding);
#endif

		_constant_binds.push_back(st_gl_uniform_block(binding));
	}

	GLint active_uniforms, longest_uniform;
	glGetProgramInterfaceiv(_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &active_uniforms);
	glGetProgramInterfaceiv(_handle, GL_UNIFORM, GL_MAX_NAME_LENGTH, &longest_uniform);

	std::string uniform_name;
	uniform_name.reserve(longest_uniform);
	for (uint32_t i = 0; i < active_uniforms; ++i)
	{
		glGetProgramResourceName(_handle, GL_UNIFORM, i, longest_uniform, NULL, uniform_name.data());
		GLint location = glGetProgramResourceLocation(_handle, GL_UNIFORM, uniform_name.c_str());

		GLint block_index;
		GLenum props[] = { GL_BLOCK_INDEX };
		glGetProgramResourceiv(_handle, GL_UNIFORM, i, 1, props, 1, NULL, &block_index);

		if (block_index < 0)
		{
			// Unintuitive, but we are interested in the binding point carried over from SPIRV-Cross, which
			// here is the value of the uniform.
			GLint binding;
			glGetUniformiv(_handle, location, &binding);

#if ST_GL_LOG_REFLECTION
			printf("\tUniform %s at location %d (block index %d)\n", uniform_name.c_str(), binding, block_index);
#endif

			_texture_binds.push_back(st_gl_uniform(location, binding));
		}
	}

	// The texture binds are not guaranteed proper order when read out from reflection.
	std::sort(_texture_binds.begin(), _texture_binds.end(), [](const st_gl_uniform& a, const st_gl_uniform& b)
	{
		return a.get_binding() < b.get_binding();
	});

	GLint active_buffers;
	glGetProgramInterfaceiv(_handle, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &active_buffers);

#if ST_GL_LOG_REFLECTION
	GLint longest_buffer;
	glGetProgramInterfaceiv(_handle, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &longest_buffer);
	std::string buffer_name;
	buffer_name.reserve(longest_buffer);
#endif

	for (uint32_t i = 0; i < active_buffers; ++i)
	{
		GLint binding;
		GLenum props[] = { GL_BUFFER_BINDING };
		glGetProgramResourceiv(_handle, GL_SHADER_STORAGE_BLOCK, i, 1, props, 1, NULL, &binding);

#if ST_GL_LOG_REFLECTION
		glGetProgramResourceName(_handle, GL_SHADER_STORAGE_BLOCK, i, longest_buffer, NULL, buffer_name.data());
		printf("\tBuffer %s at location %d\n", buffer_name.c_str(), binding);
#endif

		_buffer_binds.push_back(st_gl_shader_storage_block(binding));
	}
}

#endif
