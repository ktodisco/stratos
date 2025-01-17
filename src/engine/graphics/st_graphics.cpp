/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/platform/dx12/st_dx12_graphics_context.h>
#include <graphics/platform/opengl/st_gl_graphics_context.h>
#include <graphics/platform/vulkan/st_vk_graphics_context.h>

#include <math/st_mat4f.h>
#include <math/st_vec2f.h>
#include <math/st_vec3f.h>

#include <system/st_window.h>

#include <cassert>
#include <memory>

st_graphics_context* st_graphics_context::_this = nullptr;

st_graphics_context* st_graphics_context::get()
{
	return _this;
}

std::unique_ptr<st_graphics_context> st_graphics_context::create(e_st_graphics_api api, const st_window* window)
{
	std::unique_ptr<st_graphics_context> context;
	
	switch (api)
	{
	case e_st_graphics_api::dx12:
		context = std::make_unique<st_dx12_graphics_context>(window);
		break;
	case e_st_graphics_api::opengl:
		context = std::make_unique<st_gl_graphics_context>(window);
		break;
	case e_st_graphics_api::vulkan:
		context = std::make_unique<st_vk_graphics_context>(window);
		break;
	default:
		break;
	}

	_this = context.get();

	return std::move(context);
}
