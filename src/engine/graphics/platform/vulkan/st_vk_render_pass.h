#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/st_framebuffer.h>

#if defined(ST_GRAPHICS_API_VULKAN)

class st_render_texture;

class st_vk_render_pass
{
	st_vk_render_pass(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil);
	~st_vk_render_pass();

	void begin();
	void end();

private:
	vk::RenderPass _render_pass;
	std::unique_ptr<st_framebuffer> _framebuffer;
};

#endif
