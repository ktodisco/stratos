#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_framebuffer.h>

typedef st_gl_framebuffer st_platform_framebuffer;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_framebuffer.h>

typedef st_dx12_framebuffer st_platform_framebuffer;
#elif defined(ST_GRAPHICS_API_VULKAN)
#include <graphics/platform/vulkan/st_vk_framebuffer.h>

typedef st_vk_framebuffer st_platform_framebuffer;
#else
#error Graphics API not defined.
#endif

class st_framebuffer : public st_platform_framebuffer
{
public:
	st_framebuffer(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil) :
		st_platform_framebuffer(count, targets, depth_stencil) {}
};
