#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_buffer.h>

typedef st_gl_buffer st_platform_buffer;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_buffer.h>

typedef st_dx12_buffer st_platform_buffer;
#elif defined(ST_GRAPHICS_API_VULKAN)
#include <graphics/platform/vulkan/st_vk_buffer.h>

typedef st_vk_buffer st_platform_buffer;
#else
#error Graphics API not defined.
#endif

class st_buffer : public st_platform_buffer
{
public:
	st_buffer(const uint32_t count, const size_t element_size, const e_st_buffer_usage_flags usage) :
		st_platform_buffer(count, element_size, usage) {}
};
