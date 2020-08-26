/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_render_texture.h>

#if defined(ST_GRAPHICS_API_VULKAN)

st_vk_render_texture::st_vk_render_texture(
	uint32_t width,
	uint32_t height,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	const st_vec4f& clear) : st_texture(width, height, 1, format, usage, initial_state)
{
}

st_vk_render_texture::~st_vk_render_texture()
{
}

#endif
