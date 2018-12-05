#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_imgui.h>

typedef st_gl_imgui st_platform_imgui;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_imgui.h>

typedef st_dx12_imgui st_platform_imgui;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

class st_imgui_graphics : public st_platform_imgui
{
};
