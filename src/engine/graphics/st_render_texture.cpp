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

	st_texture_view_desc rtv_desc;
	rtv_desc._texture = _texture.get();
	rtv_desc._format = _format;
	rtv_desc._first_mip = 0;
	rtv_desc._mips = 1;
	rtv_desc._usage = e_st_view_usage::render_target;
	_rtv = context->create_texture_view(rtv_desc);

	st_texture_view_desc srv_desc;
	srv_desc._texture = _texture.get();
	srv_desc._format = _format;
	srv_desc._first_mip = 0;
	srv_desc._mips = 1;
	_srv = context->create_texture_view(srv_desc);
}

st_render_texture::~st_render_texture()
{
	_texture = nullptr;
	_rtv = nullptr;
	_srv = nullptr;
}
