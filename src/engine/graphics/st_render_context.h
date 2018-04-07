#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_render_context.h>

typedef st_gl_render_context st_platform_render_context;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_render_context.h>

typedef st_dx12_render_context st_platform_render_context;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

class st_render_context : public st_platform_render_context
{
public:
	st_render_context(const class st_window* window) : st_platform_render_context(window) {}
};
