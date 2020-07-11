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
		const vk::RenderPass& pass,
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil);
	~st_vk_framebuffer();

	const vk::Framebuffer& get() const { return _framebuffer; }
	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }

private:
	vk::Framebuffer _framebuffer;

	// TODO: Delete these when render pass begin takes a desc that defines the clear values and viewports.
	uint32_t _width = 0;
	uint32_t _height = 0;
};

#endif
