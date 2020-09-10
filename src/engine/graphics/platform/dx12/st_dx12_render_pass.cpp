/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_render_pass.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/st_render_context.h>
#include <graphics/st_render_texture.h>

#include <graphics/platform/dx12/st_dx12_framebuffer.h>

st_dx12_render_pass::st_dx12_render_pass(
	uint32_t count,
	class st_render_texture** targets,
	class st_render_texture* depth_stencil)
{
	// Naively, create the viewport from the first target.
	if (count > 0)
	{
		D3D12_VIEWPORT b;

		_viewport =
		{
			0,
			0,
			FLOAT(targets[0]->get_width()),
			FLOAT(targets[0]->get_height()),
			0.0f,
			1.0f,
		};
	}
	_framebuffer = std::make_unique<st_dx12_framebuffer>(
		count,
		targets,
		depth_stencil);
}

st_dx12_render_pass::~st_dx12_render_pass()
{
	_framebuffer = nullptr;
}

void st_dx12_render_pass::begin(
	st_render_context* context,
	st_vec4f* clear_values,
	const uint8_t clear_count)
{
	context->set_viewport(_viewport);
	_framebuffer->bind(context);
}

void st_dx12_render_pass::end(class st_render_context* context)
{
}

#endif