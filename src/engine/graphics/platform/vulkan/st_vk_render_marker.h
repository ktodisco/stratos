#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <string>

class st_vk_render_marker
{
public:
	st_vk_render_marker(const std::string& message);
	~st_vk_render_marker();
};

#endif
