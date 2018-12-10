/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_constant_buffer.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_render_context.h>
#include <graphics/st_shader.h>

#include <math/st_vec2f.h>
#include <math/st_vec3f.h>
#include <math/st_mat4f.h>

#include <cassert>

st_gl_constant_buffer::st_gl_constant_buffer(const size_t size) : _size(size)
{
	glGenBuffers(1, &_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _buffer);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

st_gl_constant_buffer::~st_gl_constant_buffer()
{
	glDeleteBuffers(1, &_buffer);
}

void st_gl_constant_buffer::add_constant(
		const std::string& name,
		const e_st_shader_constant_type constant_type)
{
	st_gl_constant constant;
	constant._name = name;
	constant._type = constant_type;

	_constants.push_back(constant);
}

void st_gl_constant_buffer::update(const st_gl_render_context* context, void* data)
{
	const st_gl_shader* shader = context->get_bound_shader();

	// Update by sending values from the data buffer piece by piece depending on type.
	size_t offset = 0;
	for (auto& constant : _constants)
	{
		st_gl_uniform uniform = shader->get_uniform(constant._name.c_str());

		switch (constant._type)
		{
		case st_shader_constant_type_float:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				float* val = reinterpret_cast<float*>(data_offset);
				uniform.set(*val);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
		case st_shader_constant_type_vec2:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_vec2f* vec2 = reinterpret_cast<st_vec2f*>(data_offset);
				uniform.set(*vec2);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
		case st_shader_constant_type_vec3:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_vec3f* vec3 = reinterpret_cast<st_vec3f*>(data_offset);
				uniform.set(*vec3);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
		case st_shader_constant_type_vec4:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_vec4f* vec4 = reinterpret_cast<st_vec4f*>(data_offset);
				uniform.set(*vec4);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
		case st_shader_constant_type_mat4:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_mat4f* mat4 = reinterpret_cast<st_mat4f*>(data_offset);
				uniform.set(*mat4);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
		case st_shader_constant_type_block:
			{
				st_gl_uniform_block block = shader->get_uniform_block(constant._name.c_str(), _size);
				block.set(_buffer, data, _size);

				offset += _size;
			}
			break;
		default:
			assert(false);
			break;
		}
	}
}

#endif
