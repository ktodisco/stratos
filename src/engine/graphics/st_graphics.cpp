/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/platform/dx12/st_dx12_render_context.h>
#include <graphics/platform/opengl/st_gl_render_context.h>

#include <math/st_mat4f.h>
#include <math/st_vec2f.h>
#include <math/st_vec3f.h>

#include <system/st_window.h>

#include <cassert>
#include <memory>

st_render_context* st_render_context::_this = nullptr;

st_render_context* st_render_context::get()
{
	return _this;
}

std::unique_ptr<st_render_context> st_render_context::create(e_st_graphics_api api, const st_window* window)
{
	std::unique_ptr<st_render_context> context;
	
	switch (api)
	{
	case e_st_graphics_api::dx12:
		context = std::make_unique<st_dx12_render_context>(window);
		break;
	case e_st_graphics_api::opengl:
		context = std::make_unique<st_gl_render_context>(window);
		break;
	case e_st_graphics_api::vulkan:
		//context = std::make_unique<st_vulkan_render_context>(window);
		break;
	default:
		break;
	}

	_this = context.get();

	return std::move(context);
}

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
