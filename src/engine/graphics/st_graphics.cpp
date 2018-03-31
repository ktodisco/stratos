/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <math/st_mat4f.h>
#include <math/st_vec2f.h>
#include <math/st_vec3f.h>

#include <cassert>

size_t st_graphics_get_shader_constant_size(e_st_shader_constant_type constant_type)
{
	size_t size = 0;

	switch (constant_type)
	{
	case st_shader_constant_type_float:
		size = sizeof(float);
	case st_shader_constant_type_vec2:
		size = sizeof(st_vec2f);
		break;
	case st_shader_constant_type_vec3:
		size = sizeof(st_vec3f);
		break;
	case st_shader_constant_type_vec4:
		size = sizeof(st_vec4f);
		break;
	case st_shader_constant_type_mat4:
		size = sizeof(st_mat4f);
		break;
	default:
		assert(false);
		break;
	}

	return size;
}
