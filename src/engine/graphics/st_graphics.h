#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#define ST_GRAPHICS_API_OPENGL
//#define ST_GRAPHICS_API_DX12
//#define ST_GRAPHICS_API_VULKAN

typedef void* st_graphics_resource;

enum e_st_clear_flags
{
	st_clear_flag_color = 1,
	st_clear_flag_depth = 2,
	st_clear_flag_stencil = 4
};

// TODO: DirectX packing rules do not allow for vec2s or vec3s.
// It's probably best just to deprecate them.
enum e_st_shader_constant_type
{
	st_shader_constant_type_float,
	st_shader_constant_type_vec2,
	st_shader_constant_type_vec3,
	st_shader_constant_type_vec4,
	st_shader_constant_type_mat4,
};

size_t st_graphics_get_shader_constant_size(e_st_shader_constant_type constant_type);

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/opengl/st_gl_graphics.h>
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/dx12/st_dx12_graphics.h>
#elif defined(ST_GRAPHICS_API_VULKAN)
#error Vulkan not implemented.
#else
#error Graphics API not defined.
#endif
