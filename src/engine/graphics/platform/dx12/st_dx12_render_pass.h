#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <memory>

class st_dx12_render_pass
{
public:
	st_dx12_render_pass(
		uint32_t count,
		class st_render_texture** targets,
		class st_render_texture* depth_stencil);
	~st_dx12_render_pass();

	void begin(
		class st_render_context* context,
		class st_vec4f* clear_values,
		const uint8_t clear_count);
	void end(class st_render_context* context);

private:
	std::unique_ptr<class st_dx12_framebuffer> _framebuffer;
	D3D12_VIEWPORT _viewport;
};

#endif
