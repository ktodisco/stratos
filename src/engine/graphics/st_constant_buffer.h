#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

// Constant buffer interface:
// constructor takes size
// call add_variable with string name and type for each variable (opengl benefit)
// update with cb struct memcpys on dx12 and name/offset meta is used for opengl
//		opengl probably needs the context passed to get current program and query locations
// commit is called with the context to update descriptor tables on DX

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_constant_buffer.h>

typedef st_gl_constant_buffer st_platform_constant_buffer;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/dx12/st_dx12_constant_buffer.h>

typedef st_dx12_constant_buffer st_platform_constant_buffer;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

class st_constant_buffer : public st_platform_constant_buffer
{
public:
	st_constant_buffer(const size_t size) : st_platform_constant_buffer(size) {}
};