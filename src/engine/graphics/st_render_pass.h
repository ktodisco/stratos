#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_render_pass.h>

typedef st_gl_render_pass st_platform_render_pass;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_render_pass.h>

typedef st_dx12_render_pass st_platform_render_pass;
#elif defined(ST_GRAPHICS_API_VULKAN)
#include <graphics/platform/vulkan/st_vk_render_pass.h>

typedef st_vk_render_pass st_platform_render_pass;
#else
#error Graphics API not defined.
#endif

/*
** Represents an API-specific render pass object, for those APIs that have one.
*/
class st_render_pass : public st_platform_render_pass
{
public:
	st_render_pass(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil) :
		st_platform_render_pass(count, targets, depth_stencil) {}
};
