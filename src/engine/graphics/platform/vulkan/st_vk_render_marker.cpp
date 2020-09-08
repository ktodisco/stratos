/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_render_marker.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>

#include <string>

st_vk_render_marker::st_vk_render_marker(const std::string& message)
{
	st_vk_render_context::get()->begin_marker(message);
}

st_vk_render_marker::~st_vk_render_marker()
{
	st_vk_render_context::get()->end_marker();
}

#endif
