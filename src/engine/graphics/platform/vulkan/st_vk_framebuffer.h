#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <cstdint>
#include <vector>

class st_vk_framebuffer
{
public:
	st_vk_framebuffer(
		vk::Device* device,
		const vk::RenderPass& pass,
		uint32_t count,
		struct st_target_desc* targets,
		struct st_target_desc* depth_stencil);
	~st_vk_framebuffer();

	void transition(class st_graphics_context* context);

	const vk::Framebuffer& get() const { return _framebuffer; }
	uint32_t get_width() const { return _width; }
	uint32_t get_height() const { return _height; }
	uint32_t get_target_count() const { return _targets.size(); }

private:
	vk::Framebuffer _framebuffer;
	std::vector<class st_render_texture*> _targets;
	class st_render_texture* _depth_stencil = nullptr;

	// TODO: Delete these when render pass begin takes a desc that defines the clear values and viewports.
	uint32_t _width = 0;
	uint32_t _height = 0;

	// Cache the device pointer for destruction.
	vk::Device* _device;
};

#endif
