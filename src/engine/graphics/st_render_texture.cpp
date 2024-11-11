/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_render_texture.h>

#include <graphics/st_graphics_context.h>

st_render_texture::st_render_texture(
	st_graphics_context* context,
	uint32_t width,
	uint32_t height,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	st_vec4f clear,
	const char* name) : _width(width), _height(height), _format(format)
{
	st_texture_desc desc;
	desc._width = width;
	desc._height = height;
	desc._levels = 1;
	desc._format = format;
	desc._usage = usage;
	desc._initial_state = initial_state;
	desc._clear._color = clear;
	_texture = context->create_texture(desc);
	context->set_texture_name(_texture.get(), name);
	_view = context->create_texture_view(_texture.get());
}

st_render_texture::~st_render_texture()
{
	_texture = nullptr;
	_view = nullptr;
}
