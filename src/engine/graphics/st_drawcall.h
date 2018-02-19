#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_drawcall.h>

typedef st_gl_static_drawcall st_platform_static_drawcall;
typedef st_gl_dynamic_drawcall st_platform_dynamic_drawcall;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/dx12/st_dx12_drawcall.h>

typedef st_dx12_static_drawcall st_platform_static_drawcall;
typedef st_dx12_dynamic_drawcall st_platform_dynamic_drawcall;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

struct st_static_drawcall : public st_platform_static_drawcall
{
};

struct st_dynamic_drawcall : public st_platform_dynamic_drawcall
{
};
