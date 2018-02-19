#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

//#define ST_GRAPHICS_API_OPENGL
#define ST_GRAPHICS_API_DX12
//#define ST_GRAPHICS_API_VULKAN

typedef void* st_graphics_resource;

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

enum e_st_texture_format
{
	st_texture_format_r8_uint = GL_RED,
	st_texture_format_r8g8b8a8_uint = GL_RGBA,
}
#elif defined(ST_GRAPHICS_API_DX12)
#include <d3d12.h>
#include <graphics/dx12/d3dx12.h>

enum e_st_primitive_topology_type
{
	st_primitive_topology_type_point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
	st_primitive_topology_type_line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
	st_primitive_topology_type_triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
};

enum e_st_primitive_topology
{
	st_primitive_topology_points = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
	st_primitive_topology_lines = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	st_primitive_topology_triangles = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	st_primitive_topology_quads = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
};

enum e_st_texture_format
{
	st_texture_format_unknown = DXGI_FORMAT_UNKNOWN,
	st_texture_format_r8_uint = DXGI_FORMAT_R8_UINT,
	st_texture_format_r8g8b8a8_unorm = DXGI_FORMAT_R8G8B8A8_UNORM,
	st_texture_format_r8g8b8a8_uint = DXGI_FORMAT_R8G8B8A8_UINT,
	st_texture_format_d24_unorm_s8_uint = DXGI_FORMAT_D24_UNORM_S8_UINT,
};
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif
