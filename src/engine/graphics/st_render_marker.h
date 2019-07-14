#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <string>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_render_marker.h>

typedef st_gl_render_marker st_platform_render_marker;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_render_marker.h>

typedef st_dx12_render_marker st_platform_render_marker;
#elif defined(ST_GRAPHICS_API_VULKAN)
#include <graphics/platform/vulkan/st_vk_render_marker.h>

typedef st_vk_render_marker st_platform_render_marker;
#else
#error Graphics API not defined.
#endif

/*
** A class that manages a scoped render group, for profiling.
**
** The intended usage is to create the marker on the stack at the top
** of the scope being profiled.
**
**		st_render_marker marker("profiling scope");
**
** The marker will clean itself up when it goes out of scope.
*/
class st_render_marker : public st_platform_render_marker
{
public:
	st_render_marker(const std::string& message) : st_platform_render_marker(message) {}
};
