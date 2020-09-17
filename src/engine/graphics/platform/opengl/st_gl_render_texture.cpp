/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_render_texture.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_texture.h>

st_gl_render_texture::st_gl_render_texture(
	uint32_t width,
	uint32_t height,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	st_vec4f clear) : st_texture(width, height, 1, format, usage, initial_state, nullptr)
{
}

st_gl_render_texture::~st_gl_render_texture()
{
}

#endif
