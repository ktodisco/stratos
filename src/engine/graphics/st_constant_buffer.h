#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_constant_buffer.h>

typedef st_gl_constant_buffer st_platform_constant_buffer;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_constant_buffer.h>

typedef st_dx12_constant_buffer st_platform_constant_buffer;
#elif defined(ST_GRAPHICS_API_VULKAN)
#include <graphics/platform/vulkan/st_vk_constant_buffer.h>

typedef st_vk_constant_buffer st_platform_constant_buffer;
#else
#error Graphics API not defined.
#endif

class st_constant_buffer : public st_platform_constant_buffer
{
public:
	st_constant_buffer(const size_t size) : st_platform_constant_buffer(size) {}
};
