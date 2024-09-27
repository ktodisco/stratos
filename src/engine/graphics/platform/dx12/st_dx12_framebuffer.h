#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cstdint>
#include <vector>

class st_dx12_framebuffer
{
public:
	st_dx12_framebuffer(
		uint32_t count,
		struct st_target_desc* targets,
		struct st_target_desc* depth_stencil);
	~st_dx12_framebuffer();

	void bind(class st_graphics_context* context);
	void unbind(class st_graphics_context* context);

private:
	uint32_t _target_count;
	std::vector<class st_render_texture*> _targets;
	class st_render_texture* _depth_stencil = nullptr;
};

#endif
