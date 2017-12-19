#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#define ST_GRAPHICS_API_OPENGL
//#define ST_GRAPHICS_API_DX12
//#define ST_GRAPHICS_API_VULKAN

enum e_st_clear_flags
{
	st_clear_flag_color = 1,
	st_clear_flag_depth = 2,
	st_clear_flag_stencil = 4
};

#if defined(ST_GRAPHICS_API_OPENGL)
#define GLEW_STATIC
#include <GL/glew.h>

enum e_st_primitive_topology
{
	st_primitive_topology_points = GL_POINTS,
	st_primitive_topology_lines = GL_LINES,
	st_primitive_topology_triangles = GL_TRIANGLES,
	st_primitive_topology_quads = GL_QUADS,
};
#elif defined(ST_GRAPHICS_API_DX12)
#error DX12 not implemented.
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif
