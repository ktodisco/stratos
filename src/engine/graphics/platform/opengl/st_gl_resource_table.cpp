/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_resource_table.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_render_context.h>
#include <graphics/platform/opengl/st_gl_texture.h>

st_gl_resource_table::st_gl_resource_table()
{
}

st_gl_resource_table::~st_gl_resource_table()
{
	glDeleteSamplers(_samplers.size(), _samplers.data());
}

void st_gl_resource_table::add_constant_buffer(st_gl_constant_buffer* cb)
{
}

void st_gl_resource_table::add_texture_resource(st_gl_texture* sr)
{
	_srvs.push_back(sr);

	// Create a sampler for the texture.
	GLuint sampler;
	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, sr->get_levels() > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, sr->get_levels() > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

	_samplers.push_back(sampler);

}

void st_gl_resource_table::add_buffer_resource(class st_gl_buffer* br)
{
}

void st_gl_resource_table::bind(st_gl_render_context* context)
{
	for (uint32_t i = 0; i < _srvs.size(); ++i)
	{
		_srvs[i]->bind(context, _samplers[i]);
	}
}

#endif
