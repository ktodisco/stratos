#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/st_shader_manager.h>

#include <cstdint>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_shader.h>

typedef st_gl_shader st_platform_shader;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/dx12/st_dx12_shader.h>

typedef st_dx12_shader st_platform_shader;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

/*
** Represents a shader.
** @see st_program
*/
class st_shader : public st_platform_shader
{
public:
	st_shader(const char* source, uint8_t type) : st_platform_shader(source, type) {}
};
