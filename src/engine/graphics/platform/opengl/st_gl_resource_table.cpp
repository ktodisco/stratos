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
}

void st_gl_resource_table::add_constant_buffer(st_gl_constant_buffer* cb)
{
}

void st_gl_resource_table::add_texture_resource(st_gl_texture* sr)
{
	_shader_resources.push_back(sr);
}

void st_gl_resource_table::add_buffer_resource(class st_gl_buffer* br)
{
}

void st_gl_resource_table::bind(st_gl_render_context* context)
{
	for (auto& sr : _shader_resources)
	{
		sr->bind(context);
	}
}

#endif
