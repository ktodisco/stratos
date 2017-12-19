#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <cstdint>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_texture.h>

typedef st_gl_texture st_platform_texture;
#elif defined(ST_GRAPHICS_API_DX12)
#error DX12 not implemented.
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

/*
** Represents a texture object for rendering.
*/
class st_texture : public st_platform_texture
{
public:
	st_texture() : st_platform_texture() {}
};
