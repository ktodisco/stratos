/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_constant_buffer.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/opengl/st_gl_render_context.h>
#include <graphics/st_shader.h>
#include <math/st_vec2f.h>
#include <math/st_vec3f.h>
#include <math/st_mat4f.h>

#include <cassert>

st_gl_constant_buffer::st_gl_constant_buffer(const size_t size)
{
}

st_gl_constant_buffer::~st_gl_constant_buffer()
{
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
		case st_shader_constant_type_vec2:
			// TODO.
			break;
		case st_shader_constant_type_vec3:
			{
				st_vec3f* vec3 = reinterpret_cast<st_vec3f*>(data) + offset;
				uniform.set(*vec3);
			}
			break;
		case st_shader_constant_type_mat4:
			{
				st_mat4f* mat4 = reinterpret_cast<st_mat4f*>(data) + offset;
				uniform.set(*mat4);
			}
			break;
		default:
			assert(false);
			break;
		}
		
		offset += st_graphics_get_shader_constant_size(constant._type);
	}
}

void st_gl_constant_buffer::commit(class st_gl_render_context* context)
{
}

#endif