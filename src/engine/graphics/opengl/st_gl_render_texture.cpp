/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_render_texture.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/opengl/st_gl_texture.h>

st_gl_render_texture::st_gl_render_texture(
	uint32_t width,
	uint32_t height,
	e_st_texture_format format,
	const st_vec4f& clear)
{
	reserve_data(width, height, format);
}

st_gl_render_texture::~st_gl_render_texture()
{
}

#endif
