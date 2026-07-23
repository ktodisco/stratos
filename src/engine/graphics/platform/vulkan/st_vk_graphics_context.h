#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>

#include <math/st_vec3f.h>
#include <math/st_vec4f.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class st_vk_graphics_context : public st_graphics_context
{
public:

	st_vk_graphics_context(const class st_window* window);
	~st_vk_graphics_context();

	std::unique_ptr<st_device> create_device(const st_device_desc& desc) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::vulkan; }
	void get_supported_formats(
		const class st_window* window,
		class st_device* device,
		std::vector<e_st_format>& formats) override;

private:

	vk::Instance _instance;

	void* _vk_library = nullptr;
};

#endif
