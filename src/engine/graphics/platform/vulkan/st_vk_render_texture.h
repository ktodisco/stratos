#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/st_texture.h>

#include <math/st_vec4f.h>

#include <cstdint>

class st_vk_render_texture : public st_texture
{
public:
	st_vk_render_texture(
		uint32_t width,
		uint32_t height,
		e_st_format format,
		e_st_texture_usage_flags usage,
		e_st_texture_state initial_state,
		const st_vec4f& clear);
	~st_vk_render_texture();
};

#endif
