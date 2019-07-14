#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <cstdint>
#include <vector>

class st_vk_framebuffer
{
public:
	st_vk_framebuffer(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil) {}
	~st_vk_framebuffer() {}

	void bind(class st_render_context* context) {}
	void unbind(class st_render_context* context) {}

private:
	uint32_t _target_count;
	std::vector<class st_render_texture*> _targets;
	class st_render_texture* _depth_stencil = nullptr;
};

#endif
