#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <cstdint>

class st_vk_pipeline_state
{
public:
	st_vk_pipeline_state(
		const struct st_pipeline_state_desc& desc,
		const class st_render_pass* render_pass);
	~st_vk_pipeline_state();

	const vk::Pipeline& get() const { return _pipeline; }

private:
	vk::Pipeline _pipeline;
};

#endif
