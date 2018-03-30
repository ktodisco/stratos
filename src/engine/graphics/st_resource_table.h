#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_resource_table.h>

typedef st_gl_resource_table st_platform_resource_table;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/dx12/st_dx12_resource_table.h>

typedef st_dx12_resource_table st_platform_resource_table;
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif

class st_resource_table : public st_platform_resource_table
{
public:
	st_resource_table() :
		st_platform_resource_table() {}
};
