#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

class st_vk_render_pass
{
public:
	st_vk_render_pass(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil);
	~st_vk_render_pass();

	void begin(
		class st_render_context* context,
		class st_vec4f* clear_values,
		const uint8_t clear_count);
	void end(class st_render_context* context);

	const vk::RenderPass& get() const { return _render_pass; }
	const vk::Viewport& get_viewport() const { return _viewport; }

private:
	vk::RenderPass _render_pass;
	std::unique_ptr<class st_vk_framebuffer> _framebuffer;
	vk::Viewport _viewport;
};

#endif
