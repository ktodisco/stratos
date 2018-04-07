#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_render_texture.h>

typedef st_gl_render_texture st_platform_render_texture;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_render_texture.h>

typedef st_dx12_render_texture st_platform_render_texture;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

class st_render_texture : public st_platform_render_texture
{
public:
	st_render_texture(
		uint32_t width,
		uint32_t height,
		e_st_texture_format format,
		st_vec4f clear) :
		st_platform_render_texture(width, height, format, clear) {}
};
